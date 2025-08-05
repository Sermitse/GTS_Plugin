#include "Managers/Explosion.hpp"

#include "Config/Config.hpp"

#include "Managers/HighHeel.hpp"
#include "Rays/Raycast.hpp"

using namespace GTS;

namespace {
	void CreateParticle(Actor* actor, NiPoint3 position, float scale) {
		GTS_PROFILE_SCOPE("Explosion: CreateParticle");
		
		if (HighHeelManager::IsWearingHH(actor)) {
			SpawnParticle(actor, 4.60f, "GTS/Effects/Footstep_High_Heel.nif", NiMatrix3(), position, scale * 2.9f, 7, nullptr);
			SpawnParticle(actor, 4.60f, "GTS/Effects/Footstep.nif", NiMatrix3(), position, scale * 2.9f, 7, nullptr); // Spawn both
			return;
		} else {
			SpawnParticle(actor, 4.60f, "GTS/Effects/Footstep.nif", NiMatrix3(), position, scale * 2.9f, 7, nullptr); // Spawn foot only
			return;
		}
	}

	void make_explosion_at(FootEvent kind, Actor* actor, NiPoint3 position, float scale) {
		if (actor) {
			switch (kind) {
				case FootEvent::Left:
				case FootEvent::Right:
				case FootEvent::Front:
				case FootEvent::Back:
					CreateParticle(actor, position, scale);
				break;
				case FootEvent::JumpLand:
					CreateParticle(actor, position, scale);
				break;
				case FootEvent::Butt:
					CreateParticle(actor, position, scale);	
				break;
			}
		}
	}

	void ApplyStateAndPerks(Actor* actor, float& scale) {
		if (actor->AsActorState()->IsSprinting()) {
			scale *= 1.25f; // Sprinting makes you seem bigger
			if (Runtime::HasPerk(actor, "GTSPerkSprintDamageMult2")) {
				scale *= 1.75f; // A lot bigger
			}
		}
		if (actor->AsActorState()->IsWalking()) {
			scale *= 0.75f; // Walking makes you seem smaller
		}
		if (actor->IsSneaking()) {
			scale *= 0.75f; // Sneaking makes you seem smaller
		}
	}
}

namespace GTS {
	ExplosionManager& ExplosionManager::GetSingleton() noexcept {
		static ExplosionManager instance;
		return instance;
	}

	std::string ExplosionManager::DebugName() {
		return "::ExplosionManager";
	}

	void ExplosionManager::OnImpact(const Impact& impact) {
		if (!impact.actor) {
			return;
		}
		GTS_PROFILE_SCOPE("ExplosionManager: OnImpact");
		auto actor = impact.actor;

		float scale = impact.scale;
		float minimal_size = 2.0f;
		
		if (actor->formID == 0x14) {
			if (HasSMT(actor)) {
				minimal_size = 1.0f;
				scale += 0.33f;
			}
		}
		if (scale > minimal_size && !actor->AsActorState()->IsSwimming()) {
			ApplyStateAndPerks(actor, scale);
			FootEvent foot_kind = impact.kind;
			
			if (foot_kind == FootEvent::JumpLand) {
				float fallmod = 1.0f + (GetFallModifier(actor) - 1.0f);
				scale *= 1.5f * fallmod; // Jumping makes you sound bigger
			}
			if (HighHeelManager::IsWearingHH(actor)) {
				scale *= GetHighHeelsBonusDamage(actor, true, 0.5f); // Wearing High Heels makes explosions bigger based on HH height
			}

			for (NiAVObject* node: impact.nodes) {
				// First try casting a ray
				NiPoint3 foot_location = node->world.translate;
				bool success = false;

				NiPoint3 ray_start = foot_location + NiPoint3(0.0, 0.0, (5.0f * scale)); // Start a bit higher
				float hh_offset = HighHeelManager::GetBaseHHOffset(actor).Length();
				float ray_length = (hh_offset + 60.0f) * scale;
				NiPoint3 ray_direction(0.0, 0.0, -1.0f);
				
				NiPoint3 explosion_pos = CastRayStatics(actor, ray_start, ray_direction, ray_length, success);

				scale *= 1.0f + (Potion_GetMightBonus(actor) * 0.5f);
				scale *= impact.modifier;

				if (!success) {
					explosion_pos = foot_location;
					explosion_pos.z = actor->GetPosition().z;
					if (foot_kind == FootEvent::Butt) {
						explosion_pos.z -= 3.0f * scale;
					}
				}
				if (actor->formID == 0x14 && Config::GetGameplay().bPlayerAnimEffects) {
					make_explosion_at(impact.kind, actor, explosion_pos, scale);
				} else if (actor->formID != 0x14 && Config::GetGameplay().bNPCAnimEffects) {
					make_explosion_at(impact.kind, actor, explosion_pos, scale);
				}
			}
		}
	}
}