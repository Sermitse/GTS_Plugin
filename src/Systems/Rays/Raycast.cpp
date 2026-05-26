#include "Systems/Rays/Raycast.hpp"
#include "Systems/Rays/AllCollector.hpp"


#include "Managers/Cameras/CamUtil.hpp"

#include "config/Config.hpp"

using namespace GTS;

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
		while (obj) {
			if (!obj->parent) break;
			obj = obj->parent;
		}

		hit.body = obj;
		if (!hit.body) return;

		const auto collisionObj = static_cast<const RE::hkpCollidable*>(hit.body);
		const auto flags = collisionObj->broadPhaseHandle.collisionFilterInfo;

		const uint64_t m = 1ULL << flags;
		constexpr uint64_t filter = 0x40122716; //TODO: replace with correct layer mask
		if ((m & filter) == 0) return;

		for (const auto* filteredObj : objectFilter) {
			if (hit.getAVObject() == filteredObj) return;
		}

		earlyOutHitFraction = hit.hitFraction;
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
		pickRayInput.to = RE::hkVector4(0.0f, 0.0f, 0.0f, 0.0f);
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
			typedef bool(__fastcall* RayCastFunType)(decltype(RE::PlayerCamera::unk120), RE::bhkWorld*, glm::vec4&, glm::vec4&, uint32_t*, RE::Character**, float);
			static auto cameraCaster = REL::Relocation<RayCastFunType>(REL::RelocationID(32270, 33007, NULL));
			res.hit = cameraCaster(cam->unk120, physicsWorld, start, end, static_cast<uint32_t*>(res.data), &res.hitCharacter, traceHullSize);
		}

		if (res.hit) {
			res.hitPos = end;
			res.rayLength = glm::length(static_cast<glm::vec3>(res.hitPos) - static_cast<glm::vec3>(start));
		}

		return res;
	}

	NiPoint3 ComputeCameraCollision(RE::Actor* cameraActor, const NiPoint3& rayStart, const NiPoint3& rayEnd, const float hullMult, const float rayMult) {

		if (!cameraActor) return rayEnd;

		// Determine hull size.
		const float Hull = (hullMult < 0.0f) ? GetFrustrumNearDistance() : camhullSize * hullMult;
		NiPoint3 currentStart = rayStart;
		NiPoint3 lastValidResult = rayEnd;

		const auto toVec4 = [](const NiPoint3& p) { return glm::vec4(p.x, p.y, p.z, 0.0f); };
		const auto hkpHitPos = [](const glm::vec4& from, const glm::vec4& to, float fraction) {
			return NiPoint3(
				(to.x - from.x) * fraction + from.x,
				(to.y - from.y) * fraction + from.y,
				(to.z - from.z) * fraction + from.z
			);
		};

		// -------------------------------------------------------------------------
		std::vector<RE::NiAVObject*> ignoreList;
		{

			auto raylen = rayMult > 0.0f ? Hull * rayMult : Hull;

			const glm::vec4 origin4 = toVec4(rayStart);
			const glm::vec4 probeEnds[6] = {
				{ rayStart.x + raylen, rayStart.y,          rayStart.z,          0.0f },
				{ rayStart.x - raylen, rayStart.y,          rayStart.z,          0.0f },
				{ rayStart.x,          rayStart.y + raylen, rayStart.z,          0.0f },
				{ rayStart.x,          rayStart.y - raylen, rayStart.z,          0.0f },
				{ rayStart.x,          rayStart.y,          rayStart.z + raylen, 0.0f },
				{ rayStart.x,          rayStart.y,          rayStart.z - raylen, 0.0f },
			};

			for (const glm::vec4& probeEnd : probeEnds) {

				HkpRayResult result = HkpCastRay(origin4, probeEnd);

				if (Config::Advanced.bShowOverlay) {
					DebugDraw::DrawLineForMS({ rayStart.x, rayStart.y, rayStart.z }, { probeEnd.x, probeEnd.y, probeEnd.z }, 50, { 1.0f, 1.0f, 0.0f, 1.0f }, 1.0f); //Yellow
				}

				for (RayCollector::HitResult& hit : result.hitArray) {
					RE::NiAVObject* av = hit.getAVObject();
					if (!av) continue;
					if (!av->GetCollisionObject()) continue;
					if (av->userData && av->userData->formID == cameraActor->formID) continue;
					NiPoint3 hitPos = hkpHitPos(origin4, probeEnd, hit.hitFraction);
					if (Config::Advanced.bShowOverlay) {
						DebugDraw::DrawLineForMS({ rayStart.x, rayStart.y, rayStart.z }, { hitPos.x, hitPos.y, hitPos.z }, 50, { 1.0f, 0.0f, 0.0f, 1.0f }, 1.5f); //Red
					}
					if (rayStart.GetDistance(hitPos) < Hull) {
						ignoreList.push_back(av);
					}
				}
			}

			std::ranges::sort(ignoreList);
			ignoreList.erase(std::ranges::unique(ignoreList).begin(), ignoreList.end());
		}

		// Keeps track of the original filter info for any object modified across frames
		absl::flat_hash_map<RE::NiAVObject*, std::uint32_t> originalFilters;
		originalFilters.reserve(8);

		{
			for (RE::NiAVObject* av : ignoreList) {
				if (!av) continue;

				bhkCollisionObject* colObj = av->GetCollisionObject();
				if (!colObj) continue;

				bhkRigidBody* bRigidBody = colObj->GetRigidBody();
				if (!bRigidBody) continue;

				hkRefPtr<hkReferencedObject> hkrefObj = bRigidBody->referencedObject;
				if (!hkrefObj) continue;

				hkReferencedObject* refObj = hkrefObj.get();
				if (!refObj) continue;

				hkpRigidBody* hRigidbody = skyrim_cast<hkpRigidBody*>(refObj);
				if (hRigidbody) {

					if (hRigidbody->world && hRigidbody->collidable.broadPhaseHandle.ownerOffset != RE::hkpTypedBroadPhaseHandle::kInvalidOffset) {

						//rcx+08 crash check
						std::uintptr_t* internalSimulationPtr = reinterpret_cast<std::uintptr_t*>(reinterpret_cast<char*>(hRigidbody) + 0xF8);

						if (internalSimulationPtr && *internalSimulationPtr != 0) {

							if (!originalFilters.contains(av)) {
								originalFilters[av] = hRigidbody->collidable.broadPhaseHandle.collisionFilterInfo;
							}

							hRigidbody->collidable.broadPhaseHandle.collisionFilterInfo |= 1 << 14;
							hRigidbody->world->UpdateCollisionFilterOnEntity(hRigidbody, hkpUpdateCollisionFilterOnEntityMode::kFullCheck, hkpUpdateCollectionFilterMode::kIncludeCollections);
						}
					}
				}
			}
		}

		// Upward check...
		const NiPoint3 upwardRayStart = currentStart;
		NiPoint3 upwardRayEnd = currentStart;
		upwardRayEnd.z += Hull;

		const glm::vec4 upwardRayStart4 = glm::vec4(upwardRayStart.x, upwardRayStart.y, upwardRayStart.z, 0.0f);
		const glm::vec4 upwardRayEnd4 = glm::vec4(upwardRayEnd.x, upwardRayEnd.y, upwardRayEnd.z, 0.0f);
		auto groundResult = RaycastAsCamera(upwardRayStart4, upwardRayEnd4, 1.0);

		if (groundResult.hit) {
			currentStart.z = groundResult.hitPos.z + 5.0f;
		}

		constexpr int maxIterations = 2;
		int iterations = 0;
		NiPoint3 ShiftedStart = currentStart;

		while (iterations < maxIterations) {
			const glm::vec4 rayStart4 = { currentStart.x, currentStart.y, currentStart.z, 0.0f };
			const glm::vec4 rayEnd4 = { rayEnd.x, rayEnd.y, rayEnd.z, 0.0f };
			const RayResult result = RaycastAsCamera(rayStart4, rayEnd4, Hull);

			if (!result.hit) {
				lastValidResult = rayEnd;
				break;
			}

			NiPoint3 ResHit = { result.hitPos.x, result.hitPos.y, result.hitPos.z };
			NiPoint3 ResNorm = { result.rayNormal.x, result.rayNormal.y, result.rayNormal.z };

			NiPoint3 Res = ResHit + (ResNorm * glm::min(result.rayLength, Hull));
			lastValidResult = Res;

			float distance = Res.GetDistance(ShiftedStart);
			if (distance > Hull * 2.0f) {
				break;
			}

			float offset = (Hull * 2.0f) - distance;
			currentStart = Res + (ResNorm * offset);
			++iterations;
		}

		// Revert the objects back to their original layers so regular game simulation is unaffected
		// Iterate through the hashmap of tracked NiAVObjects
		for (auto it = originalFilters.begin(); it != originalFilters.end(); ++it) {
			NiAVObject* av = it->first;
			if (!av) continue;

			bhkCollisionObject* colObj = av->GetCollisionObject();
			if (!colObj) continue;

			bhkRigidBody* bRigidBody = colObj->GetRigidBody();
			if (!bRigidBody) continue;

			hkRefPtr<hkReferencedObject> hkrefObj = bRigidBody->referencedObject;
			if (!hkrefObj) continue;

			hkReferencedObject* refObj = hkrefObj.get();
			if (!refObj) continue;

			hkpRigidBody* hRigidbody = skyrim_cast<hkpRigidBody*>(refObj);
			if (hRigidbody) {

				if (hRigidbody->world && hRigidbody->collidable.broadPhaseHandle.ownerOffset != RE::hkpTypedBroadPhaseHandle::kInvalidOffset) {

					//rcx+08 crash check
					std::uintptr_t* internalSimulationPtr = reinterpret_cast<std::uintptr_t*>(reinterpret_cast<char*>(hRigidbody) + 0xF8);

					if (internalSimulationPtr && *internalSimulationPtr != 0) {
						hRigidbody->collidable.broadPhaseHandle.collisionFilterInfo &= ~(1 << 14);
						hRigidbody->world->UpdateCollisionFilterOnEntity(hRigidbody, hkpUpdateCollisionFilterOnEntityMode::kFullCheck, hkpUpdateCollectionFilterMode::kIncludeCollections);
					}
				}
			}
		}
		return lastValidResult;
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