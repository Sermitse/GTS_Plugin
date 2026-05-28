#include "Systems/Rays/Camera/CameraCollision.hpp"
#include "CameraRayCollector.hpp"
#include "Managers/Cameras/CamUtil.hpp"
#include "config/Config.hpp"

namespace {

	constexpr int FibRayCnt = 8;

	//---- Compile time fibonacci sequence calc, used to evenly spread out the ignoreList rays

	consteval float ce_sqrt(float x) {
		float r = x * 0.5f;
		for (int i = 0; i < 32; ++i) {
			r = 0.5f * (r + x / r);
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

	CamRayResult RaycastAsCamera(glm::vec4 start, glm::vec4 end, float traceHullSize) noexcept {

		CamRayResult res;

		const auto ply = RE::PlayerCharacter::GetSingleton();
		const auto cam = RE::PlayerCamera::GetSingleton();
		if (!ply->parentCell || !cam->unk120) return res;

		auto physicsWorld = ply->parentCell->GetbhkWorld();
		if (physicsWorld) {
			typedef bool(__fastcall* RayCastFunType)(decltype(RE::PlayerCamera::unk120), RE::bhkWorld*, glm::vec4&, glm::vec4&, hkpRootCdPoint*, RE::Character**, float);
			static auto cameraCaster = REL::Relocation<RayCastFunType>(REL::RelocationID(32270, 33007, NULL));
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
		const bhkWorld* physicsWorld = cell->GetbhkWorld();

		NiPoint3 currentStart = rayStart;
		NiPoint3 finalCameraPosition = rayEnd;
		// Determine hull size.
		//The hull used for tracing extends from the center of the ray.
		const float Hull = (hullMult < 0.0f) ? GetFrustrumNearDistance() / 2.f : defaultCamHullSize * hullMult;
		const float Hullx2 = Hull * 2.0f;
		const float RayLen = rayMult > 0.0f ? Hull * rayMult : Hull;
		const float actorZPos = cameraActor->GetPositionZ();

		const auto toVec4 = [](const NiPoint3& p) { return glm::vec4(p.x, p.y, p.z, 0.0f); };
		const auto hkpHitPos = [](const glm::vec4& from, const glm::vec4& to, float fraction) {
			return NiPoint3 {
				(to.x - from.x) * fraction + from.x,
				(to.y - from.y) * fraction + from.y,
				(to.z - from.z) * fraction + from.z
			};
		};

		// -------------------------------------------------------------------------
		std::vector<RE::hkpCdBody*> ignoreList;
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
					DebugDraw::DrawLineForMS({ currentStart.x, currentStart.y, currentStart.z }, { probeEnd.x, probeEnd.y, probeEnd.z }, 16, { 1.0f, 1.0f, 1.0f, 1.0f }, 0.1f); //White
				}

				for (CamRayCollector::HitResult& hit : result.hitArray) {

					NiPoint3 hitPos = hkpHitPos(origin4, probeEnd, hit.hitFraction);

					if (const auto* collidable = static_cast<const hkpCollidable*>(hit.body)) {
						const auto layer = static_cast<COL_LAYER>(collidable->broadPhaseHandle.collisionFilterInfo & 0x7F);

						//As defined in skyrim.esm minus anything character related.
						if (
							layer == COL_LAYER::kUnidentified ||
							layer == COL_LAYER::kStatic ||
							layer == COL_LAYER::kAnimStatic ||
							layer == COL_LAYER::kTransparent ||
							layer == COL_LAYER::kTrees ||
							layer == COL_LAYER::kTerrain ||
							layer == COL_LAYER::kTrap ||
							layer == COL_LAYER::kCloudTrap ||
							layer == COL_LAYER::kGround ||
							layer == COL_LAYER::kDebrisSmall ||
							layer == COL_LAYER::kTransparentSmallAnim ||
							layer == COL_LAYER::kItemPicker ||
							layer == COL_LAYER::kLOS ||
							layer == COL_LAYER::kUnused0 ||
							layer == COL_LAYER::kUnused1
							) {
							//Hits must be higher than ground, otherwise we disable ground meshes that aren't actually intefering.
							if (hitPos.z > actorZPos) {
								//Hits
								if (Config::Advanced.bShowOverlay) {
									DebugDraw::DrawLineForMS({ currentStart.x, currentStart.y, currentStart.z }, { hitPos.x, hitPos.y, hitPos.z }, 16, { 0.0f, 0.0f, 1.0f, 1.0f }, 0.5f); //Blue
								}

								ignoreList.push_back(const_cast<hkpCdBody*>(hit.body));
							}
						}
					}
				}
			}
		}

		std::ranges::sort(ignoreList);
		ignoreList.erase(std::ranges::unique(ignoreList).begin(), ignoreList.end());

		absl::flat_hash_map<RE::hkpCdBody*, std::uint32_t> originalFilters;
		originalFilters.reserve(8); //Should be enough

		//Disable collision for all valid hit objects for the duration of this function
		{
			for (RE::hkpCdBody* av : ignoreList) {
				if (!av) continue;

				const hkpCollidable* collidable = static_cast<const hkpCollidable*>(av);
				const auto bpType = static_cast<hkpWorldObject::BroadPhaseType>(collidable->broadPhaseHandle.type);
				hkpWorldObject* worldObj = collidable->GetOwner<hkpWorldObject>();

				if (!worldObj) continue;

				if (bpType == hkpWorldObject::BroadPhaseType::kEntity) {
					hkpEntity* entity = static_cast<hkpEntity*>(worldObj);
					if (entity->collidable.broadPhaseHandle.ownerOffset != RE::hkpTypedBroadPhaseHandle::kInvalidOffset) {
						if (!originalFilters.contains(av)) {
							originalFilters[av] = entity->collidable.broadPhaseHandle.collisionFilterInfo;
						}

						{
							BSWriteLockGuard lock(physicsWorld->worldLock);

							entity->collidable.broadPhaseHandle.collisionFilterInfo |= (1 << 14);
							entity->world->UpdateCollisionFilterOnEntity(
								entity,
								hkpUpdateCollisionFilterOnEntityMode::kDisableEntityEntityCollisionsOnly,
								hkpUpdateCollectionFilterMode::kIgnoreCollections
							);
						}
					}
				}
				else if (bpType == hkpWorldObject::BroadPhaseType::kPhantom) {
					hkpPhantom* phantom = static_cast<hkpPhantom*>(worldObj);
					if (phantom->collidable.broadPhaseHandle.ownerOffset != RE::hkpTypedBroadPhaseHandle::kInvalidOffset) {
						if (!originalFilters.contains(av)) {
							originalFilters[av] = phantom->collidable.broadPhaseHandle.collisionFilterInfo;
						}

						{
							BSWriteLockGuard lock(physicsWorld->worldLock);

							phantom->collidable.broadPhaseHandle.collisionFilterInfo |= 1 << 14;
							phantom->world->UpdateCollisionFilterOnPhantom(phantom, hkpUpdateCollectionFilterMode::kIgnoreCollections);
						}
					}
				}
			}
		}

		{
			//Underground prevention
			//Force raystart to be atleast at the same z pos as hullx2 + char controller position. 
			//prevents tracking bones that are underground,
			if (currentStart.z < actorZPos + Hullx2) {
				currentStart.z = actorZPos + Hullx2;
			}
		}

		//Floor clearance
		{
			const glm::vec4 floorStart4 = { currentStart.x, currentStart.y, currentStart.z, 0.0f };
			const glm::vec4 floorEnd4 = { currentStart.x, currentStart.y, currentStart.z - Hull, 0.0f };
			const CamRayResult floorResult = RaycastAsCamera(floorStart4, floorEnd4, 1.0f); //Hull should be thin here.


			if (floorResult.hit && floorResult.rayLength < Hull) {
				currentStart.z += Hull - floorResult.rayLength;
			}
				

			if (Config::Advanced.bShowOverlay) {
				DebugDraw::DrawLineForMS({ floorStart4.x, floorStart4.y, floorStart4.z }, { currentStart.x, currentStart.y, currentStart.z }, 16, { 0.0f, 1.0f, 1.0f, 1.0f }, 1.5f); //Blue
			}
		}

		//Ceiling clearance
		{
			const glm::vec4 ceilStart4 = { currentStart.x, currentStart.y, currentStart.z, 0.0f };
			const glm::vec4 ceilEnd4 = { currentStart.x, currentStart.y, currentStart.z + Hull, 0.0f };
			const CamRayResult ceilResult = RaycastAsCamera(ceilStart4, ceilEnd4, 1.0f); //Hull should be thin here.

			if (ceilResult.hit && ceilResult.rayLength < Hull) {
				currentStart.z -= Hull - ceilResult.rayLength;
			}

			if (Config::Advanced.bShowOverlay) {
				DebugDraw::DrawLineForMS({ ceilStart4.x, ceilStart4.y, ceilStart4.z }, { currentStart.x, currentStart.y, currentStart.z }, 16, { 0.0f, 1.0f, 1.0f, 1.0f }, 1.5f); //Blue
			}

		}

		//Fire camera raycast
		{
			constexpr int maxIterations = 3;
			int iterations = 0;
			NiPoint3 ShiftedStart = currentStart;

			while (iterations < maxIterations) {
				const glm::vec4 rayStart4 = { currentStart.x, currentStart.y, currentStart.z, 0.0f };
				const glm::vec4 rayEnd4 = { rayEnd.x, rayEnd.y, rayEnd.z, 0.0f };
				const CamRayResult result = RaycastAsCamera(rayStart4, rayEnd4, Hull);

				if (!result.hit) break;

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


		// Revert the objects back to their original collision filter data.
		for (auto it = originalFilters.begin(); it != originalFilters.end(); ++it) {
			hkpCdBody* av = it->first;
			if (!av) continue;

			const hkpCollidable* collidable = static_cast<const hkpCollidable*>(av);
			const auto bpType = static_cast<hkpWorldObject::BroadPhaseType>(collidable->broadPhaseHandle.type);
			hkpWorldObject* worldObj = collidable->GetOwner<hkpWorldObject>();

			if (!worldObj) continue;

			if (bpType == hkpWorldObject::BroadPhaseType::kEntity) {
				hkpEntity* entity = static_cast<hkpEntity*>(worldObj);
				if (entity->collidable.broadPhaseHandle.ownerOffset != RE::hkpTypedBroadPhaseHandle::kInvalidOffset) {

					{
						BSWriteLockGuard lock(physicsWorld->worldLock);
						entity->collidable.broadPhaseHandle.collisionFilterInfo = it->second;
						entity->world->UpdateCollisionFilterOnEntity(
							entity,
							hkpUpdateCollisionFilterOnEntityMode::kDisableEntityEntityCollisionsOnly,
							hkpUpdateCollectionFilterMode::kIgnoreCollections
						);
					}
				}
			}
			else if (bpType == hkpWorldObject::BroadPhaseType::kPhantom) {
				hkpPhantom* phantom = static_cast<hkpPhantom*>(worldObj);
				if (phantom->collidable.broadPhaseHandle.ownerOffset != RE::hkpTypedBroadPhaseHandle::kInvalidOffset) {

					{
						BSWriteLockGuard lock(physicsWorld->worldLock);

						phantom->collidable.broadPhaseHandle.collisionFilterInfo = it->second;
						phantom->world->UpdateCollisionFilterOnPhantom(phantom, hkpUpdateCollectionFilterMode::kIgnoreCollections);
					}
				}
			}
		}

		if (Config::Camera.bDoCameraSmoooth) {
			// Exponential smooth toward the raw result
			static NiPoint3 smoothed = finalCameraPosition;
			const float dt = RE::GetSecondsSinceLastFrame();
			const float alpha = 1.0f - std::expf(-Config::Camera.fCameraSmoothDelta * dt); 
			smoothed.x += (finalCameraPosition.x - smoothed.x) * alpha;
			smoothed.y += (finalCameraPosition.y - smoothed.y) * alpha;
			smoothed.z += (finalCameraPosition.z - smoothed.z) * alpha;
			return smoothed;
		}

		return finalCameraPosition;

	}
}