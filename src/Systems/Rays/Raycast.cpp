#include "Systems/Rays/Raycast.hpp"
#include "Systems/Rays/AllCollector.hpp"


#include "Managers/Cameras/CamUtil.hpp"

using namespace GTS;

namespace {

	bool FilterCollisionOut(const RE::hkpCollidable& collidable, bool filter = false) {
		try {
			// Check if collidable is valid before calling GetShape()
			auto shape = collidable.GetShape();

			// Extra validation for shape
			if (shape && shape != nullptr) {
				auto shapeType = shape->type;

				// Only access userData if it exists
				if (shape->userData && shape->userData != nullptr) {
					auto materialID = shape->userData->materialID;
					logger::info("ShapeTypeID: {}, Material: {}",
						static_cast<std::uint32_t>(shapeType),
						static_cast<std::uint32_t>(materialID));
				}
				else {
					logger::info("ShapeTypeID: {}, No userData available",
						static_cast<std::uint32_t>(shapeType));
				}
			}
			else {
				logger::info("No valid shape found");
			}

		}
		catch (...) {
			logger::info("Exception caught in FilterCollisionOut");
		}
		return filter;
	}

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

		for (auto& ray_result: collector->GetHits()) {
			ray_result.position = MeterToGameUnit(origin + normed * length * ray_result.hitFraction);
		}
		std::ranges::sort(collector->GetHits(), [](const AllRayCollectorOutput &a, const AllRayCollectorOutput &b){
			return a.hitFraction < b.hitFraction;
		});
	}
}

namespace GTS {

	RayResult CastCamRay(glm::vec4 start, glm::vec4 end, float traceHullSize) noexcept {

		RayResult res;

		const auto ply = RE::PlayerCharacter::GetSingleton();
		const auto cam = RE::PlayerCamera::GetSingleton();
		if (!ply->parentCell || !cam->unk120) return res;

		auto physicsWorld = ply->parentCell->GetbhkWorld();
		if (physicsWorld) {
			typedef bool(__fastcall* RayCastFunType)(
				decltype(RE::PlayerCamera::unk120) physics, RE::bhkWorld* world, glm::vec4& rayStart,
				glm::vec4& rayEnd, uint32_t* rayResultInfo, RE::Character** hitCharacter, float traceHullSize
				);
			static auto cameraCaster = REL::Relocation<RayCastFunType>(RELOCATION_ID(32270, 33007));
			res.hit = cameraCaster(
				cam->unk120, physicsWorld,
				start, end, static_cast<uint32_t*>(res.data), &res.hitCharacter,
				traceHullSize
			);
		}

		if (res.hit) {
			res.hitPos = end;
			res.rayLength = glm::length(static_cast<glm::vec3>(res.hitPos) - static_cast<glm::vec3>(start));
		}

		return res;
	}

	NiPoint3 ComputeRaycast(const NiPoint3& rayStart, const NiPoint3& rayEnd, const float hullMult) {
		// Determine hull size.
		const float Hull = (hullMult < 0.0f) ? GetFrustrumNearDistance() : camhullSize * hullMult;
		NiPoint3 currentStart = rayStart;
		// Default to rayEnd if no hit occurs
		NiPoint3 lastValidResult = rayEnd;

		// Perform an UPWARD ray cast (along Z-axis) to find ground level ABOVE rayStart's XY position.
		NiPoint3 upwardRayStart = currentStart;
		NiPoint3 upwardRayEnd = currentStart;

		upwardRayEnd.z += Hull;

		// Convert to 4D vectors for raycast.
		const auto upwardRayStart4 = glm::vec4(upwardRayStart.x, upwardRayStart.y, upwardRayStart.z, 0.0f);
		const auto upwardRayEnd4 = glm::vec4(upwardRayEnd.x, upwardRayEnd.y, upwardRayEnd.z, 0.0f);

		auto groundResult = CastCamRay(upwardRayStart4, upwardRayEnd4, 1.0); // Hull size doesn't matter much for ground check

		if (groundResult.hit) {
			// Ground hit detected *above*. Adjust currentStart (rayStart) Z position to be at ground level (plus a small offset).
			currentStart.z = groundResult.hitPos.z + 5.0f; 
		}

		constexpr int maxIterations = 2;
		int iterations = 0;

		auto ShiftedStart = currentStart;

		while (iterations < maxIterations) {
			// Convert current start to 4D vectors for the raycast.
			const auto rayStart4 = glm::vec4(currentStart.x, currentStart.y, currentStart.z, 0.0f);
			const auto rayEnd4 = glm::vec4(rayEnd.x, rayEnd.y, rayEnd.z, 0.0f);
			const auto result = CastCamRay(rayStart4, rayEnd4, Hull);

			// If no hit, return rayEnd.
			if (!result.hit) {
				return rayEnd;
			}

			// Get hit position and normal.
			NiPoint3 ResHit = { result.hitPos.x, result.hitPos.y, result.hitPos.z };
			NiPoint3 ResNorm = { result.rayNormal.x, result.rayNormal.y, result.rayNormal.z };

			// Compute a new result point along the normal.
			NiPoint3 Res = ResHit + (ResNorm * glm::min(result.rayLength, Hull));

			// Store this as our latest valid result
			lastValidResult = Res;

			// Check the distance from the original start.
			float distance = Res.GetDistance(ShiftedStart); // Use the *original* rayStart for distance check
			if (distance > Hull * 2.0f) {
				return Res;  // The hit is far enough.
			}

			// Otherwise, compute the extra offset needed.
			float offset = (Hull * 2.0f) - distance;

			// Nudge the start point further along the normal.
			currentStart = Res + (ResNorm * offset);
			++iterations;
		}

		// If we exceed max iterations, return the last computed valid result.
		return lastValidResult;
	}


	NiPoint3 CastRay(TESObjectREFR* ref, const NiPoint3& origin, const NiPoint3& direction, const float& length, bool& success) {
		auto collector = AllRayCollector::Create();
		collector->Reset();
		collector->filterInfo = bhkCollisionFilter::GetSingleton()->GetNewSystemGroup() << 16 | std::to_underlying(COL_LAYER::kLOS);
		CastRayImpl(ref, origin, direction, length, collector.get());

		if (collector->HasHit()) {
			for (auto& hit: collector->GetHits()) {
				// This varient just returns the first result
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
			for (auto& hit: collector->GetHits()) {
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
