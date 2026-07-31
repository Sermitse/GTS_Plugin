#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Damage/Utils/SizeDamageUtils.hpp"
#include "Managers/Damage/CollisionDamage.hpp"

#include "Hooks/Experiments/Experiments_FootColliders.hpp"

#include "Config/Config.hpp"

#include "Managers/Damage/SizeHitEffects.hpp"
#include "Managers/Damage/TinyCalamity.hpp"
#include "Managers/Audio/GoreAudio.hpp"
#include "Managers/CrushManager.hpp"
#include "Managers/GTSSizeManager.hpp"

#include "Magic/Effects/Common.hpp"
#include "Managers/Contact/ColliderUtils.hpp"

#include "Utils/DeathReport.hpp"
#include "Utils/MovementForce.hpp"


using namespace GTS;

namespace GTS {
	float CollisionDamage::ToHavok() {
		const float toHavok = 1.0f / *reinterpret_cast<const float*>(RE::Offset::Havok::WorldScaleInverse.address());
		return toHavok;
	}
	void CollisionDamage::DebugCollision(RE::bhkWorld* world, Actor* actor, std::vector<NiPoint3> CoordsToCheck, float maxFootDistance, float toHavok, bool condition) {
		if (condition) {
			constexpr int duration = 300;
				for (const auto& footPoint : CoordsToCheck) {
					DebugDraw::DrawSphere(glm::vec3(footPoint.x, footPoint.y, footPoint.z), maxFootDistance, duration);
				}
				for (auto& otherActor : find_actors()) {
					if (otherActor == actor) continue;
					if (auto shapeData = GetControllerShapeData(otherActor, toHavok)) {
						BSReadLockGuard lock(world->worldLock);
						DebugDrawShape(shapeData->shape, shapeData->transform, shapeData->angleZ, 1.0f / toHavok, duration);
					}
				}
			}
		}
	
	bool CollisionDamage::HasCollided(Actor* actor, Actor* otherActor, RE::bhkWorld* world, std::vector<NiPoint3> CoordsToCheck, NiPoint3 giantLocation, float giantScale, float SCALE_RATIO, float maxFootDistance, float maxCheckDistanceSq, float sphereRadiusSq, float toHavok) {
		// Broad-phase distance cull
		NiPoint3 diff = otherActor->GetPosition() - giantLocation;
		float distanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
		if (distanceSq > maxCheckDistanceSq) return false;

		// Scale ratio cull
		float tinyScale = get_visual_scale(otherActor) * GetSizeFromBoundingBox(otherActor);
		if (giantScale / tinyScale <= SCALE_RATIO) return false;
		if (auto shapeData = GetControllerShapeData(otherActor, toHavok)) {
			BSReadLockGuard lock(world->worldLock);
			for (const NiPoint3& footPt : CoordsToCheck) {
				hkVector4 center;
				center.quad = _mm_mul_ps(_mm_set_ps(0.0f, footPt.z, footPt.y, footPt.x), _mm_set1_ps(toHavok));
				if (SphereOverlapsShape(shapeData->shape, center, sphereRadiusSq, shapeData->transform, shapeData->angleZ)) {
					return true;
					break;
				}
			}
		} else {
			// Fallback for ragdolled/dead actors with no controller
			NiPoint3 actorLocation = otherActor->GetPosition();
			for (const NiPoint3& footPt : CoordsToCheck) {
				if ((footPt - actorLocation).Length() - Collision_Distance_Override <= maxFootDistance) {
					return true;
					break;
				}
			}
		}
		return false;
	}
	// Safer optimization that preserves original behavior
	void CollisionDamage::DoFootCollision(Actor* actor, float damage, float radius, int random, float bbmult, float crush_threshold, DamageSource Cause, bool Right, bool ApplyCooldown, bool ignore_rotation, bool SupportCalamity) {

		if (!actor) return;
		
		float giantScale = get_visual_scale(actor) * GetSizeFromBoundingBox(actor);
		constexpr float BASE_CHECK_DISTANCE = 180.0f;
		const float toHavok = ToHavok();
		float SCALE_RATIO = 1.15f;
		float Calamity = 1.0f;

		bool SMT = TinyCalamityActive(actor);
		if (SMT) {
			if (SupportCalamity) Calamity = 6.0f;
			giantScale += 0.20f;
			SCALE_RATIO = 0.7f;
		}

		float maxFootDistance = radius * giantScale;
		float maxFootDistance_Calamity = maxFootDistance * Calamity;

		std::vector<NiPoint3> CoordsToCheck = GetFootCoordinates(actor, Right, ignore_rotation);
		if (CoordsToCheck.empty()) return;

		auto* world = actor->GetParentCell() ? actor->GetParentCell()->GetbhkWorld() : nullptr;
		if (!world) return;

		if (!IsIdleDamage(Cause)) {
			const bool Condition = DebugDraw::CanDraw(actor, DebugDraw::DrawTarget::kAnyGTS);
			DebugCollision(world, actor, CoordsToCheck, maxFootDistance, toHavok, Condition);
			if (SMT) {
				DebugCollision(world, actor, CoordsToCheck, maxFootDistance * Calamity, toHavok, Condition);
			}
		}

		NiPoint3 giantLocation = actor->GetPosition();
		float maxCheckDistance = BASE_CHECK_DISTANCE * giantScale;
		float maxCheckDistanceSq = maxCheckDistance * maxCheckDistance;
		
		const float sphereRadiusHk = maxFootDistance * toHavok;
		const float sphereRadiusSq = sphereRadiusHk * sphereRadiusHk;
		const float sphereRadiusHk_Calamity = maxFootDistance_Calamity * toHavok;
		const float sphereRadiusSq_Calamity = sphereRadiusHk_Calamity * sphereRadiusHk_Calamity;

		for (auto& otherActor : find_actors()) {
			if (otherActor == actor) continue;

			bool HitDetected = HasCollided(actor, otherActor, world, CoordsToCheck, giantLocation, giantScale, SCALE_RATIO, maxFootDistance, maxCheckDistanceSq, sphereRadiusSq, toHavok);

			// Calamity has larger hit range, but said range is used only for shrinking, it's NOT causing any damage and can't crush/kill
			if (SupportCalamity && SMT) {
				bool HitDetected_SMT = HasCollided(actor, otherActor, world, CoordsToCheck, giantLocation, giantScale, SCALE_RATIO, maxFootDistance_Calamity, maxCheckDistanceSq, sphereRadiusSq_Calamity, toHavok);
				if (HitDetected_SMT) {
					if (ApplyCooldown) { // Needed to fix Thigh Crush stuff
						bool OnCooldown = IsActionOnCooldown(otherActor, CooldownSource::Damage_Thigh);
						if (!OnCooldown) {
							Utils_PushCheck(actor, otherActor, Get_Bone_Movement_Speed(actor, Cause)); // pass original un-altered force
							DoSizeDamage(actor, otherActor, damage, 0.0f, 10, 0, Cause, false);
							ApplyActionCooldown(actor, CooldownSource::Damage_Thigh);
						}
					} else {
						Utils_PushCheck(actor, otherActor, Get_Bone_Movement_Speed(actor, Cause)); // pass original un-altered force
						DoSizeDamage(actor, otherActor, damage, 0.0f, 10, 0, Cause, false);
					}
				}
			}

			if (HitDetected) { // This is normal collision that deals damage and can crush/kill
				if (ApplyCooldown) {
					if (!IsActionOnCooldown(otherActor, CooldownSource::Damage_Thigh)) {
						Utils_PushCheck(actor, otherActor, Get_Bone_Movement_Speed(actor, Cause));
						DoSizeDamage(actor, otherActor, damage, bbmult, crush_threshold, random, Cause, true);
						ApplyActionCooldown(otherActor, CooldownSource::Damage_Thigh);
					}
				} else {
					Utils_PushCheck(actor, otherActor, Get_Bone_Movement_Speed(actor, Cause));
					DoSizeDamage(actor, otherActor, damage, bbmult, crush_threshold, random, Cause, true);
				}
			}
		}
	}

	void CollisionDamage::DoSizeDamage(Actor* giant, Actor* tiny, float damage, float bbmult, float crush_threshold, int random, DamageSource Cause, bool apply_damage) { // Applies damage and crushing
		GTS_PROFILE_SCOPE("CollisionDamage: DoSizeDamage");
		if (!giant) {
			return;
		}
		if (!tiny) {
			return;
		}
		if (giant == tiny) {
			return;
		}
		if (!tiny->Is3DLoaded() || !giant->Is3DLoaded()) {
			return;
		}
		if (!CanDoDamage(giant, tiny, true) || IsBetweenBreasts(tiny)) { // disallow 
			return;
		}
		float size_difference = get_scale_difference(giant, tiny, SizeType::VisualScale, false, true);
		float damage_result = CalculateSizeDamage(giant, tiny, Cause, damage);
		
		TinyCalamity_ShrinkActor(giant, tiny, damage_result * 0.35f);

		if (!tiny->IsDead()) {
			float experience = std::clamp(damage_result/500, 0.0f, 0.05f);
			ModSizeExperience(giant, experience);
		}

		if (tiny->IsPlayerRef() && GetAV(tiny, ActorValue::kStamina) > 2.0f) {
			DamageAV(tiny, ActorValue::kStamina, damage_result * 2.0f);
			damage_result -= GetAV(tiny, ActorValue::kStamina); // Reduce damage by stamina amount
			damage_result = std::max<float>(damage_result, 0);

			if (damage_result < GetAV(tiny, ActorValue::kStamina)) {
				return; // Fully protect against size-related damage
			}
		}
		if (apply_damage) {
			SizeHitEffects::PerformInjuryDebuff(giant, tiny, damage_result * bbmult, random);
			// ^ Chance to break bonues and inflict additional damage, as well as making target more vulerable to size damage
			ModVulnerability(giant, tiny, damage_result);
			InflictSizeDamage(giant, tiny, damage_result);
			CrushCheck(giant, tiny, size_difference, crush_threshold, Cause);
		}
	}
}