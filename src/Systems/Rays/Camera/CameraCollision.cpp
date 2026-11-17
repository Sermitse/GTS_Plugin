#include "Systems/Rays/Camera/CameraCollision.hpp"
#include "CameraRayCollector.hpp"
#include "Managers/Cameras/CamUtil.hpp"
#include "Managers/Collision/DynamicCollisionUtils.hpp"
#include "config/Config.hpp"

namespace {

	constexpr int FibRayCnt = 12;

	//---- Compile time golden ratio calc, used to evenly spread out the ignoreList rays

	consteval float ce_sqrt(float x) {
		float r = x * 0.5f;           // r = 0 when x = 0
		for (int i = 0; i < 32; ++i) {
			r = 0.5f * (r + x / r);   // x/0
		}
		return r;
	}

	consteval float ce_sin(float x) {
		// range reduce to [-pi, pi]
		while (x > std::numbers::pi_v<float>) x -= 2.0f * std::numbers::pi_v<float>;
		while (x < -std::numbers::pi_v<float>) x += 2.0f * std::numbers::pi_v<float>;
		// Taylor: x - x^3/6 + x^5/120 - x^7/5040 + x^9/362880
		const float x2 = x * x;
		return x * (1.0f - x2 * (1.0f / 6.0f - x2 * (1.0f / 120.0f - x2 * (1.0f / 5040.0f - x2 * (1.0f / 362880.0f)))));
	}

	consteval float ce_cos(float x) {
		return ce_sin(x + std::numbers::pi_v<float> * 0.5f);
	}

	struct Vec3 { float x, y, z; };

	consteval Vec3 fibDir(int i, int n) {
		const float z = 1.0f - (i + 0.5f) * (2.0f / static_cast<float>(n));
		const float r = ce_sqrt(1.0f - z * z);
		const float theta = 2.0f * std::numbers::pi_v<float> *static_cast<float>(i) / std::numbers::phi_v<float>;
		return { r * ce_cos(theta), r * ce_sin(theta), z };
	}

	template<int N>
	consteval std::array<Vec3, N> makeFibSphere() {
		std::array<Vec3, N> dirs{};
		for (int i = 0; i < N; ++i)
			dirs[i] = fibDir(i, N);
		return dirs;
	}

	constexpr auto kFibDirs = makeFibSphere<FibRayCnt>();
}

namespace GTS::CameraCol {

	CamRayResult RaycastAsCamera(RE::Actor* cameraActor, glm::vec4 start, glm::vec4 end, float traceHullSize) noexcept {

		CamRayResult res;

		const auto cam = RE::PlayerCamera::GetSingleton();
		if (!cameraActor || !cam) return res;
		if (!cameraActor->parentCell || !cam->unk120) return res;

		auto physicsWorld = cameraActor->parentCell->GetbhkWorld();
		if (physicsWorld) {
			//cameraCaster truncates end in place to the actual impact point on hit, that truncated
			//value is what we read back below as the hit position, not the original ray endpoint.
			typedef bool(__fastcall* RayCastFunType)(decltype(RE::PlayerCamera::unk120), RE::bhkWorld*, glm::vec4&, glm::vec4&, hkpRootCdPoint*, RE::Character**, float);
			static auto cameraCaster = REL::Relocation<RayCastFunType>(Hooks::RelocationIDEx(32270, 33007, 33007, NULL).address());
			res.hit = cameraCaster(cam->unk120, physicsWorld, start, end, &res.cdPoint, &res.hitCharacter, traceHullSize);
		}

		if (res.hit) {
			res.hitPos = end;
			res.rayLength = glm::length(static_cast<glm::vec3>(res.hitPos) - static_cast<glm::vec3>(start));
		}

		return res;
	}

	NiPoint3 ComputeCameraCollision(RE::Actor* cameraActor, const NiPoint3& rayStart, const NiPoint3& rayEnd, const float hullMult, const float rayMult) {

		if (!cameraActor) return rayEnd;

		auto cell = cameraActor->GetParentCell();
		if (!cell) return rayEnd;

		//Get the world, used for a write lock later.
		bhkWorld* physicsWorld = cell->GetbhkWorld();
		if (!physicsWorld) return rayEnd;
		RE::bhkCollisionFilter* filter = static_cast<RE::bhkCollisionFilter*>(physicsWorld->GetWorld2()->collisionFilter);

		NiPoint3 currentStart = rayStart;
		NiPoint3 finalCameraPosition = rayEnd;
		// Determine hull size.
		//The hull used for tracing extends from the center of the ray.
		const float Hull = (hullMult < 0.0f) ? GetFrustumNearDistance() / 2.f : defaultCamHullSize * hullMult;
		const float Hullx2 = Hull * 2.0f;
		const float RayLen = rayMult > 0.0f ? Hull * 1.15f * rayMult : Hull * 1.15f; //Small Buffer
		const float actorZPos = cameraActor->GetPositionZ();

		const auto toVec4 = [](const NiPoint3& p) { return glm::vec4(p.x, p.y, p.z, 0.0f); };
		const auto toNiPoint = [](const glm::vec4& v) { return NiPoint3(v.x, v.y, v.z); };
		const auto drawLine = [](const NiPoint3& a_from, const NiPoint3& a_to, const glm::vec4& a_color, float a_thickness) {
			DebugDraw::DrawLineForMS({ a_from.x, a_from.y, a_from.z }, { a_to.x, a_to.y, a_to.z }, 16, a_color, a_thickness);
		};

		const auto hkpHitPos = [](const glm::vec4& from, const glm::vec4& to, float fraction) {
			return NiPoint3 {
				(to.x - from.x) * fraction + from.x,
				(to.y - from.y) * fraction + from.y,
				(to.z - from.z) * fraction + from.z
			};
		};

		// -------------------------------------------------------------------------
		absl::InlinedVector<RE::hkpCdBody*, 32> ignoreList;
		{
			const glm::vec4 origin4 = toVec4(currentStart);

			glm::vec4 probeEnds[FibRayCnt];
			for (int i = 0; i < FibRayCnt; ++i) {
				const ::Vec3& d = kFibDirs[i];
				probeEnds[i] = {
					currentStart.x + d.x * RayLen,
					currentStart.y + d.y * RayLen,
					currentStart.z + d.z * RayLen,
					0.0f
				};
			}

			for (const glm::vec4& probeEnd : probeEnds) {

				HkpRayResult result = HkpCastRay(cameraActor, origin4, probeEnd);

				//Rays
				if (Config::Advanced.bShowOverlay) {
					drawLine(currentStart, toNiPoint(probeEnd), { 1.0f, 1.0f, 1.0f, 1.0f }, 0.1f); //White
				}

				const uint64_t cameraCollidesWithBitfield = filter->layerBitfields[static_cast<uint8_t>(COL_LAYER::kCamera)];
				for (CamRayCollector::HitResult& hit : result.hitArray) {

					NiPoint3 hitPos = hkpHitPos(origin4, probeEnd, hit.hitFraction);

					if (const auto* collidable = static_cast<const hkpCollidable*>(hit.body)) {
						const auto layer = static_cast<COL_LAYER>(collidable->broadPhaseHandle.collisionFilterInfo & 0x7F);

						
						const uint64_t layerBit = 1ULL << static_cast<uint64_t>(layer);

						if (cameraCollidesWithBitfield & layerBit) {
							if (hitPos.z > actorZPos) {
								if (Config::Advanced.bShowOverlay) {
									drawLine(currentStart, hitPos, { 0.0f, 0.0f, 1.0f, 1.0f }, 0.5f);
								}
								ignoreList.push_back(const_cast<hkpCdBody*>(hit.body));
							}
						}
					}
				}
			}
		}

		std::ranges::sort(ignoreList, std::less<RE::hkpCdBody*>{});
		ignoreList.erase(std::ranges::unique(ignoreList).begin(), ignoreList.end());

		//Disable collision for all valid hit objects for the duration of this function
		absl::InlinedVector<COL_LAYER, 32> disabledLayers;

		{
			for (RE::hkpCdBody* av : ignoreList) {
				if (!av) continue;
				const hkpCollidable* collidable = static_cast<const hkpCollidable*>(av);
				COL_LAYER layer = static_cast<COL_LAYER>(collidable->broadPhaseHandle.collisionFilterInfo & 0x7F);
				if (std::ranges::find(disabledLayers, layer) == disabledLayers.end()) {
					disabledLayers.push_back(layer);
				}
			}

			{
				BSWriteLockGuard lock(physicsWorld->worldLock);

				constexpr uint64_t camBit = 1ULL << static_cast<uint64_t>(COL_LAYER::kCamera);

				for (COL_LAYER layer : disabledLayers) {
					const uint64_t layerBit = 1ULL << static_cast<uint64_t>(layer);
					filter->layerBitfields[static_cast<uint8_t>(COL_LAYER::kCamera)] &= ~layerBit;
					filter->layerBitfields[static_cast<uint8_t>(layer)] &= ~camBit;
				}
			}
		}

		//Verified start
		//Use the havok char controller to build a "safe" point.
		//To then raycast from this point onto the actual wanted postion.
		//And move along the hit ray's normal if the desired position can not be reached.
		float safeZ = actorZPos + Hullx2;
		{
			NiPoint3 anchor = {};
			float halfHeight = 0.0f;

			if (GetControllerExtent(cameraActor, anchor, halfHeight)) {

				//A shrunk actor's own half height can be smaller than the camera hull, so the anchor
				//never sits closer to the ground than the hull the sweep will use.
				anchor.z += std::max(halfHeight, Hullx2);
				safeZ = anchor.z;

				if (Config::Advanced.bShowOverlay) {
					drawLine(anchor, currentStart, { 1.0f, 0.0f, 1.0f, 1.0f }, 1.0f); //Magenta
				}

				constexpr int maxChecks = 3;
				for (int check = 0; check < maxChecks; ++check) {

					const CamRayResult verify = RaycastAsCamera(cameraActor, toVec4(anchor), toVec4(currentStart), Hull);

					if (!verify.hit) {
						break;
					}

					const NiPoint3 hitPos = { verify.hitPos.x, verify.hitPos.y, verify.hitPos.z };
					const NiPoint3 hitNorm = { verify.cdPoint.normal.x, verify.cdPoint.normal.y, verify.cdPoint.normal.z };

					currentStart = hitPos + (hitNorm * Hull);

					if (Config::Advanced.bShowOverlay) {
						drawLine(hitPos, currentStart, { 1.0f, 0.50f, 0.0f, 1.0f }, 1.5f); //Orange
					}
				}
			}
			else if (currentStart.z < safeZ) {
				//No readable controller shape. Fall back to the old z only clamp.
				currentStart.z = safeZ;
			}
		}

		//Floor clearance
		{
			const glm::vec4 floorStart4 = { currentStart.x, currentStart.y, currentStart.z, 0.0f };
			const glm::vec4 floorEnd4 = { currentStart.x, currentStart.y, currentStart.z - Hull, 0.0f };
			const CamRayResult floorResult = RaycastAsCamera(cameraActor, floorStart4, floorEnd4, 1.0f); //Hull should be thin here.


			if (floorResult.hit && floorResult.rayLength < Hull) {
				currentStart.z += Hull - floorResult.rayLength;
			}

			if (Config::Advanced.bShowOverlay) {
				drawLine(toNiPoint(floorStart4), currentStart, { 0.0f, 1.0f, 1.0f, 1.0f }, 1.5f); //Blue
			}
		}

		//Ceiling clearance
		{
			const glm::vec4 ceilStart4 = { currentStart.x, currentStart.y, currentStart.z, 0.0f };
			const glm::vec4 ceilEnd4 = { currentStart.x, currentStart.y, currentStart.z + Hull, 0.0f };
			const CamRayResult ceilResult = RaycastAsCamera(cameraActor, ceilStart4, ceilEnd4, 1.0f); //Hull should be thin here.

			//Clamped against the verified start: from inside geometry every upward hit reads as a
			//ceiling, and the correction would drive the camera further down.
			if (ceilResult.hit && ceilResult.rayLength < Hull) {
				currentStart.z = std::max(currentStart.z - (Hull - ceilResult.rayLength), safeZ);
			}

			if (Config::Advanced.bShowOverlay) {
				drawLine(toNiPoint(ceilStart4), currentStart, { 0.0f, 1.0f, 1.0f, 1.0f }, 1.5f); //Blue
			}

		}

		//Fire camera raycast
		{
			constexpr int maxIterations = 3;
			int iterations = 0;
			NiPoint3 ShiftedStart = currentStart;

			while (iterations < maxIterations) {

				ShiftedStart = currentStart;

				const glm::vec4 rayStart4 = { currentStart.x, currentStart.y, currentStart.z, 0.0f };
				const glm::vec4 rayEnd4 = { rayEnd.x, rayEnd.y, rayEnd.z, 0.0f };
				const CamRayResult result = RaycastAsCamera(cameraActor, rayStart4, rayEnd4, Hull);

				if (!result.hit) {
					finalCameraPosition = rayEnd;
					break;
				}

				const NiPoint3 ResHit = { result.hitPos.x, result.hitPos.y, result.hitPos.z };
				const NiPoint3 ResNorm = { result.cdPoint.normal.x, result.cdPoint.normal.y, result.cdPoint.normal.z };
				const NiPoint3 Res = ResHit + (ResNorm * Hull);

				const float distance = Res.GetDistance(ShiftedStart);
				if (distance > Hullx2) {
					finalCameraPosition = Res;
					break;
				}

				const float offset = Hullx2 - distance;
				currentStart = Res + (ResNorm * offset);
				finalCameraPosition = currentStart;
				++iterations;
			}
		}

		//Post-sweep floor safety, the sweep normal can push finalCameraPosition underground
		//and its XY may no longer match the pre-sweep floor sample above
		{
			const glm::vec4 floorStart4 = { finalCameraPosition.x, finalCameraPosition.y, finalCameraPosition.z, 0.0f };
			const glm::vec4 floorEnd4 = { finalCameraPosition.x, finalCameraPosition.y, finalCameraPosition.z - Hull, 0.0f };
			const CamRayResult floorResult = RaycastAsCamera(cameraActor, floorStart4, floorEnd4, 1.0f);

			if (floorResult.hit && floorResult.rayLength < Hull) {
				finalCameraPosition.z += Hull - floorResult.rayLength;
			}

			if (Config::Advanced.bShowOverlay) {
				drawLine(toNiPoint(floorStart4), finalCameraPosition, { 0.0f, 1.0f, 0.0f, 1.0f }, 1.5f); //Green
			}
		}

		// Revert the objects back to their original collision filter data.
		{

			BSWriteLockGuard lock(physicsWorld->worldLock);

			constexpr uint64_t camBit = 1ULL << static_cast<uint64_t>(COL_LAYER::kCamera);
			for (COL_LAYER layer : disabledLayers) {
				const uint64_t layerBit = 1ULL << static_cast<uint64_t>(layer);
				filter->layerBitfields[static_cast<uint8_t>(COL_LAYER::kCamera)] |= layerBit;
				filter->layerBitfields[static_cast<uint8_t>(layer)] |= camBit;
			}
		}

		return finalCameraPosition;

	}
}