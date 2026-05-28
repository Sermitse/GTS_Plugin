#include "Systems/Rays/Raycast.hpp"
#include "Systems/Rays/AllCollector.hpp"


#include "Managers/Cameras/CamUtil.hpp"

#include "config/Config.hpp"

using namespace GTS;


namespace v2 {

	consteval float ce_sqrt(float x) {
		float r = x * 0.5f;
		for (int i = 0; i < 32; ++i)
			r = 0.5f * (r + x / r);
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
		return ce_sin(x + std::numbers::pi_v<float> *0.5f);
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
}

static constexpr auto kFibDirs64 = v2::makeFibSphere<16>();

namespace {

	void CastRayImpl(TESObjectREFR* ref, const NiPoint3& in_origin, const NiPoint3& direction, const float& unit_length, AllRayCollector* collector) {
		float length = GameUnitToMeter(unit_length);
		if (!ref) {
			return;
		}
		auto cell = ref->GetParentCell();
		if (!cell) {
			return;
		}
		auto collision_world = cell->GetbhkWorld();
		if (!collision_world) {
			return;
		}
		bhkPickData pick_data;

		NiPoint3 origin = GameUnitToMeter(in_origin);
		pick_data.rayInput.from = origin;

		NiPoint3 normed = direction / direction.Length();
		NiPoint3 end = origin + normed * length;
		pick_data.rayInput.to = end;

		NiPoint3 delta = end - origin;
		pick_data.ray = delta; // Length in each axis to travel

		pick_data.rayInput.enableShapeCollectionFilter = false; // Don't bother testing child shapes
		pick_data.rayInput.filterInfo = collector->filterInfo;

		pick_data.rayHitCollectorA8 = collector;

		collision_world->PickObject(pick_data);

		for (auto& ray_result : collector->GetHits()) {
			ray_result.position = MeterToGameUnit(origin + normed * length * ray_result.hitFraction);
		}
		std::ranges::sort(collector->GetHits(), [](const AllRayCollectorOutput& a, const AllRayCollectorOutput& b) {
			return a.hitFraction < b.hitFraction;
		});
	}
}

namespace GTS {

	void RayCollector::AddRayHit(const RE::hkpCdBody& body, const RE::hkpShapeRayCastCollectorOutput& hitInfo) {
		HitResult hit{};
		hit.hitFraction = hitInfo.hitFraction;
		hit.normal = {
			hitInfo.normal.quad.m128_f32[0],
			hitInfo.normal.quad.m128_f32[1],
			hitInfo.normal.quad.m128_f32[2]
		};

		const RE::hkpCdBody* obj = &body;
		while (obj && obj->parent)
			obj = obj->parent;

		hit.body = obj;
		if (!hit.body) 
			return;

		earlyOutHitFraction = 1.0f; // Never early-out, collect everything
		hits.push_back(hit);
	}

	const std::vector<RayCollector::HitResult>& RayCollector::GetHits() {
		return hits;
	}

	void RayCollector::Reset() {
		earlyOutHitFraction = 1.0f;
		hits.clear();
		objectFilter.clear();
	}

	RE::NiAVObject* RayCollector::HitResult::getAVObject() const {
		return body ? GetHkpAVObject(body) : nullptr;
	}

	RE::NiAVObject* GetHkpAVObject(const RE::hkpCdBody* body) {
		typedef RE::NiAVObject* (*_GetUserData)(const RE::hkpCdBody*);
		static auto getAVObject = REL::Relocation<_GetUserData>(RELOCATION_ID(76160, 77988));
		return body ? getAVObject(body) : nullptr;
	}

	HkpRayResult HkpCastRay(const glm::vec4& start, const glm::vec4& end) noexcept {
		constexpr float hkpScale = 0.0142875f;
		const glm::vec4 dif = end - start;
		constexpr float one = 1.0f;

		const auto from = start * hkpScale;
		const auto to = dif * hkpScale;

		RE::hkpWorldRayCastInput pickRayInput{};
		pickRayInput.from = RE::hkVector4(from.x, from.y, from.z, one);
		pickRayInput.to = RE::hkVector4(to.x, to.y, to.z, one);  // 'to' here = end * hkpScale
		pickRayInput.filterInfo = std::to_underlying(COL_LAYER::kCamera); //TODO Fix this.
		pickRayInput.enableShapeCollectionFilter = false;
		//pickRayInput.filterInfo = bhkCollisionFilter::GetSingleton()->GetNewSystemGroup() << 16 | std::to_underlying(COL_LAYER::kCamera) | std::to_underlying(COL_LAYER::kCameraSphere);

		RayCollector collector;
		collector.Reset();

		RE::bhkPickData pickData{};
		pickData.rayInput = pickRayInput;
		pickData.ray = RE::hkVector4(to.x, to.y, to.z, one);
		pickData.rayHitCollectorA8 = reinterpret_cast<RE::hkpClosestRayHitCollector*>(&collector);


		const auto ply = RE::PlayerCharacter::GetSingleton();
		auto cell = ply->GetParentCell();
		if (!cell) return {};

		try {
			auto physicsWorld = cell->GetbhkWorld();
			if (physicsWorld) physicsWorld->PickObject(pickData);
		}
		catch (...) {}

		HkpRayResult result;
		result.hitArray = collector.GetHits();

		// Find the closest hit to determine rayLength.
		float bestFraction = 1.0f;
		for (const auto& hit : result.hitArray) {
			if (hit.hitFraction < bestFraction) {
				bestFraction = hit.hitFraction;
			}
		}

		if (bestFraction < 1.0f) {
			result.hit = true;
			result.rayLength = glm::length(glm::vec3(dif)) * bestFraction;
		}

		return result;
	}


	RayResult RaycastAsCamera(glm::vec4 start, glm::vec4 end, float traceHullSize) noexcept {

		RayResult res;

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

		const bhkWorld* physicsWorld = cell->GetbhkWorld();
		

		NiPoint3 currentStart = rayStart;
		NiPoint3 lastValidResult = rayEnd;
		// Determine hull size.
		//The hull used for tracing extends from the center of the ray. meaning that passing fnear distance doubles the clearance. this is incorrect.
		const float Hull = (hullMult < 0.0f) ? GetFrustrumNearDistance() / 2.f : defaultCamHullSize * hullMult;
		const float Hullx2 = Hull * 2.0f;
		//const float Hullx2lerped = std::lerp(Hull, Hullx2, std::clamp(rayMult, 0.0f, 1.0f));
		const float Hulllerped = std::lerp(1.f, Hull, std::clamp(rayMult, 0.0f, 1.0f));

		const float RayLen = rayMult > 0.0f ? Hull * rayMult : Hull;
		//const float RayLen = Hull;
		const float actorZPos = cameraActor->GetPositionZ();
		/*{
			//Underground prevention
			//Force raystart to the same z pos as hull + char controller position. prevents tracking bones that are underground,
			//Whichever is less, should always be raylen under normal circuimstances.
			//This is to prevent the ground the actor is walking on from being hit when small.
			const float offset = std::min(RayLen, Hull);

			if (currentStart.z < actorZPos + offset) {
				currentStart.z = actorZPos + offset;
			}
		}*/

		const auto toVec4 = [](const NiPoint3& p) { return glm::vec4(p.x, p.y, p.z, 0.0f); };
		const auto hkpHitPos = [](const glm::vec4& from, const glm::vec4& to, float fraction) {
			return NiPoint3(
				(to.x - from.x) * fraction + from.x,
				(to.y - from.y) * fraction + from.y,
				(to.z - from.z) * fraction + from.z
			);
		};

		// -------------------------------------------------------------------------
		std::vector<RE::hkpCdBody*> ignoreList;
		{
			const glm::vec4 origin4 = toVec4(currentStart);

			glm::vec4 probeEnds[16];
			for (int i = 0; i < 16; ++i) {
				const v2::Vec3& d = kFibDirs64[i];
				probeEnds[i] = { 
					currentStart.x + d.x * RayLen,
					currentStart.y + d.y * RayLen,
					currentStart.z + d.z * RayLen,
					0.0f 
				};
			}

			for (const glm::vec4& probeEnd : probeEnds) {

				HkpRayResult result = HkpCastRay(origin4, probeEnd);

				//Rays
				if (Config::Advanced.bShowOverlay) {
					DebugDraw::DrawLineForMS({ currentStart.x, currentStart.y, currentStart.z }, { probeEnd.x, probeEnd.y, probeEnd.z }, 16, { 1.0f, 1.0f, 1.0f, 1.0f }, 0.1f); //White
				}

				for (RayCollector::HitResult& hit : result.hitArray) {

					NiPoint3 hitPos = hkpHitPos(origin4, probeEnd, hit.hitFraction);

					if (const auto* collidable = static_cast<const hkpCollidable*>(hit.body)) {
						const auto layer = static_cast<COL_LAYER>(collidable->broadPhaseHandle.collisionFilterInfo & 0x7F);

						//As defined in skyrim.esm minus anything character related.
						if (
						    layer == COL_LAYER::kUnidentified         ||
							layer == COL_LAYER::kStatic               ||
							layer == COL_LAYER::kAnimStatic           ||
							layer == COL_LAYER::kTransparent          ||
							layer == COL_LAYER::kTrees                ||
							layer == COL_LAYER::kTerrain              ||
							layer == COL_LAYER::kTrap                 ||
							layer == COL_LAYER::kCloudTrap            ||
							layer == COL_LAYER::kGround               ||
							layer == COL_LAYER::kDebrisSmall          ||
							layer == COL_LAYER::kTransparentSmallAnim ||
							layer == COL_LAYER::kItemPicker           ||
							layer == COL_LAYER::kLOS                  ||
							layer == COL_LAYER::kUnused0              ||
							layer == COL_LAYER::kUnused1
						) {
							//Hits must be higher than gnd otherwise we disable ground meshes that are fine.
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
		originalFilters.reserve(32);

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

		// Hard floor clamp
		if (currentStart.z < actorZPos + Hullx2)
			currentStart.z = actorZPos + Hullx2;

		// Pre-pass: floor clearance
		{
			const glm::vec4 floorStart4 = { currentStart.x, currentStart.y, currentStart.z, 0.0f };
			const glm::vec4 floorEnd4 = { currentStart.x, currentStart.y, currentStart.z - Hull, 0.0f };
			const RayResult floorResult = RaycastAsCamera(floorStart4, floorEnd4, 1.0f);


			if (floorResult.hit && floorResult.rayLength < Hull)
				currentStart.z += (Hull - floorResult.rayLength);

			if (Config::Advanced.bShowOverlay) {
				DebugDraw::DrawLineForMS({ floorStart4.x, floorStart4.y, floorStart4.z }, { currentStart.x, currentStart.y, currentStart.z }, 16, { 0.0f, 1.0f, 1.0f, 1.0f }, 1.5f); //Blue
			}
		}

		// Pre-pass: ceiling clearance
		{
			const glm::vec4 ceilStart4 = { currentStart.x, currentStart.y, currentStart.z, 0.0f };
			const glm::vec4 ceilEnd4 = { currentStart.x, currentStart.y, currentStart.z + Hull, 0.0f };
			const RayResult ceilResult = RaycastAsCamera(ceilStart4, ceilEnd4, 1.0f);

			if (ceilResult.hit && ceilResult.rayLength < Hull)
				currentStart.z -= (Hull - ceilResult.rayLength);

			if (Config::Advanced.bShowOverlay) {
				DebugDraw::DrawLineForMS({ ceilStart4.x, ceilStart4.y, ceilStart4.z }, { currentStart.x, currentStart.y, currentStart.z }, 16, { 0.0f, 1.0f, 1.0f, 1.0f }, 1.5f); //Blue
			}

		}


		constexpr int maxIterations = 1;
		int iterations = 0;
		NiPoint3 ShiftedStart = currentStart;

		/*while (iterations < maxIterations) {
			const glm::vec4 rayStart4 = { currentStart.x, currentStart.y, currentStart.z, 0.0f };
			const glm::vec4 rayEnd4 = { rayEnd.x, rayEnd.y, rayEnd.z, 0.0f };
			const RayResult result = RaycastAsCamera(rayStart4, rayEnd4, Hull);

			if (!result.hit) break;

			const NiPoint3 ResHit = { result.hitPos.x, result.hitPos.y, result.hitPos.z };
			const NiPoint3 ResNorm = { result.cdPoint.normal.x, result.cdPoint.normal.y, result.cdPoint.normal.z };

			const NiPoint3 Res = ResHit + (ResNorm * glm::min(result.rayLength, Hull));
			lastValidResult = Res;

			const float distance = Res.GetDistance(ShiftedStart);
			if (distance > Hullx2) {
				lastValidResult = Res;
				break;
			}

			const float offset = Hullx2 - distance;
			currentStart = Res + (ResNorm * offset);
			++iterations;
		}*/

		while (iterations < maxIterations) {
			const glm::vec4 rayStart4 = { currentStart.x, currentStart.y, currentStart.z, 0.0f };
			const glm::vec4 rayEnd4 = { rayEnd.x, rayEnd.y, rayEnd.z, 0.0f };
			const RayResult result = RaycastAsCamera(rayStart4, rayEnd4, Hull);

			if (!result.hit) break;

			const NiPoint3 ResHit = { result.hitPos.x, result.hitPos.y, result.hitPos.z };
			const NiPoint3 ResNorm = { result.cdPoint.normal.x, result.cdPoint.normal.y, result.cdPoint.normal.z };
			const NiPoint3 Res = ResHit + (ResNorm * Hull);

			const float distance = Res.GetDistance(ShiftedStart);
			if (distance > Hullx2) {
				lastValidResult = Res;
				break;
			}

			const float offset = Hullx2 - distance;
			currentStart = Res + (ResNorm * offset);
			lastValidResult = currentStart;
			++iterations;
		}


		// Revert the objects back to their original layers so regular game simulation is unaffected
		// Iterate through the hashmap of tracked NiAVObjects
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
		/*// Exponential smooth toward the raw result -- frame-rate independent
		static NiPoint3 smoothed = lastValidResult;
		const float dt = RE::GetSecondsSinceLastFrame();
		const float alpha = 1.0f - std::expf(-50.0f * dt); // 12.0f = tune this
		smoothed.x += (lastValidResult.x - smoothed.x) * alpha;
		smoothed.y += (lastValidResult.y - smoothed.y) * alpha;
		smoothed.z += (lastValidResult.z - smoothed.z) * alpha;
		return smoothed;*/

		/*static NiPoint3 smoothed = lastValidResult;
		const float dt = RE::GetSecondsSinceLastFrame();

		if (lastValidResult != rayEnd) {
			// Geometry was hit, smooth the pull-back
			const float alpha = 1.0f - std::expf(-12.0f * dt);
			smoothed.x += (lastValidResult.x - smoothed.x) * alpha;
			smoothed.y += (lastValidResult.y - smoothed.y) * alpha;
			smoothed.z += (lastValidResult.z - smoothed.z) * alpha;
		}
		else {
			smoothed = lastValidResult; // no hit, track instantly
		}

		return smoothed;*/

		/*static NiPoint3 smoothed = lastValidResult;
		const float dt = RE::GetSecondsSinceLastFrame();

		const float curDepth = lastValidResult.GetDistance(rayStart);
		const float prevDepth = smoothed.GetDistance(rayStart);

		// Pull in slowly, release instantly
		const float k = curDepth < prevDepth ? 12.0f : 40.0f;
		const float alpha = 1.0f - std::expf(-k * dt);
		smoothed.x += (lastValidResult.x - smoothed.x) * alpha;
		smoothed.y += (lastValidResult.y - smoothed.y) * alpha;
		smoothed.z += (lastValidResult.z - smoothed.z) * alpha;

		return smoothed;*/

		static NiPoint3 smoothed = lastValidResult;
		const float dt = RE::GetSecondsSinceLastFrame();

		const float curDepth = lastValidResult.GetDistance(rayStart);
		const float prevDepth = smoothed.GetDistance(rayStart);

		// Pull in slowly, release instantly
		const float k = curDepth < prevDepth ? 10.0f : 50.0f;
		const float alpha = 1.0f - std::expf(-k * dt);
		smoothed.x += (lastValidResult.x - smoothed.x) * alpha;
		smoothed.y += (lastValidResult.y - smoothed.y) * alpha;
		smoothed.z += (lastValidResult.z - smoothed.z) * alpha;

		return smoothed;

	}


	NiPoint3 CastRay(TESObjectREFR* ref, const NiPoint3& origin, const NiPoint3& direction, const float& length, bool& success) {
		auto collector = AllRayCollector::Create();
		collector->Reset();
		collector->filterInfo = bhkCollisionFilter::GetSingleton()->GetNewSystemGroup() << 16 | std::to_underlying(COL_LAYER::kLOS);
		CastRayImpl(ref, origin, direction, length, collector.get());

		if (collector->HasHit()) {
			for (auto& hit : collector->GetHits()) {
				// This variant just returns the first result
				success = true;
				return hit.position;
			}
		}

		success = false;
		return {};
	}

	NiPoint3 CastRayStatics(TESObjectREFR* ref, const NiPoint3& origin, const NiPoint3& direction, const float& length, bool& success) {
		auto collector = AllRayCollector::Create();
		collector->Reset();
		collector->filterInfo = bhkCollisionFilter::GetSingleton()->GetNewSystemGroup() << 16 | std::to_underlying(COL_LAYER::kLOS);
		CastRayImpl(ref, origin, direction, length, collector.get());

		if (collector->HasHit()) {
			for (auto& hit : collector->GetHits()) {
				// This varient filters out the char ones

				auto collision_layer = static_cast<COL_LAYER>(hit.rootCollidable->broadPhaseHandle.collisionFilterInfo & 0x7F);
				//bool FilteredOut = FilterCollisionOut(*hit.rootCollidable);
				int layer_as_int = static_cast<int>(collision_layer);

				if (collision_layer != COL_LAYER::kCharController && collision_layer != COL_LAYER::kWeapon &&
					layer_as_int != 56) {
					// 8 = kBiped
					// 56 = Supposedly weapon collisions
					/*if (ref->IsPlayerRef()) {
						logger::info("------Hitting Layer: {}, as int: {}", collision_layer, layer_as_int); // Weapons hit "unknown" layer :/
					}*/
					success = true;
					return hit.position;
				}
			}
		}

		success = false;
		return {};
	}
}