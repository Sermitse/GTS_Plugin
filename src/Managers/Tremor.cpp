#include "Managers/Tremor.hpp"

#include "Config/Config.hpp"

#include "Managers/Rumble.hpp"

using namespace GTS;

namespace {
	enum Formula {
		Power,
		Smooth,
		SoftCore,
		Linear,
		Unknown,
	};

	float falloff_calc(float x, float half_power) {
		float n_falloff = 2.0f;
		return 1/(1+pow(pow(1/0.5f-1,n_falloff)*(x)/half_power,half_power));
	}

	void DoJumpingRumble(Actor* actor, float tremor, float halflife, std::string_view node_name, float duration) { 
		// This function is needed since normally jumping doesn't stack with footsteps
		// And we want to use separate footstep logic for normal walk since footsteps happen too fast and rumble manager behaves a bit incorrectly
		std::string_view tag = std::format("Tremor_{}_{}_{}", actor->formID, node_name, Time::WorldTimeElapsed());
		float fallmod = 1.0f + (GetFallModifier(actor) - 1.0f);

		Rumbling::Once(tag, actor, tremor * fallmod, halflife, node_name, duration);
	}

	void UpdateTremorValues(Actor* actor, FootEvent kind, float& tremor) {
		if (actor) {
			if (actor->AsActorState()->IsSprinting()) {
				tremor *= 1.10f; // Sprinting makes tremor stronger
			}
			if (actor->AsActorState()->IsWalking()) {
				tremor *= 0.80f; // Walking makes tremor weaker
			}
			if (actor->IsSneaking()) {
				tremor *= 0.80f; // Sneaking makes tremor weaker
			}
			if (kind == FootEvent::JumpLand) {
				tremor *= Rumble_Default_JumpLand; // Jumping makes tremor stronger
			}

			tremor *= GetHighHeelsBonusDamage(actor, true);
		}
	}
}

namespace GTS {

	std::string TremorManager::DebugName() {
		return "::TremorManager";
	}

	void TremorManager::OnImpact(const Impact& impact) { // This Tremor is used for regular footsteps, not custom GTS attacks
		if (impact.actor) {
			GTS_PROFILE_SCOPE("TremorManager: OnImpact");

			auto actor = impact.actor;
			if (actor) {
				
				float tremor = Rumble_Default_FootWalk * 0.35f;
				float duration = 1.25f;
				float calamity = 1.0f;

				float threshold = 1.25f; // tremor starts to appear past this scale
				float size = impact.scale;

				if (actor->IsPlayerRef()) {
					tremor *= 1.20f; // slightly stronger footstep tremor for player
					if (HasSMT(actor)) {
						threshold = 0.55f;
						calamity = 1.8f;
					}
				} 

				if (tremor > 1e-5) {
					if (!actor->AsActorState()->IsSwimming() && size > threshold) {
						UpdateTremorValues(actor, impact.kind, tremor);

						for (NiAVObject* node: impact.nodes) {
							if (node) {
								const bool npcEffects = Config::Gameplay.bNPCAnimEffects;
								const bool pcEffects = Config::Gameplay.bPlayerAnimEffects;

								if (actor->IsPlayerRef() && pcEffects) {
									if (impact.kind == FootEvent::JumpLand) { // let Rumble Manager handle it.
										DoJumpingRumble(actor, tremor * calamity, 0.03f, node->name, duration);
									} else {
										ApplyShakeAtPoint(actor, tremor * calamity, node->world.translate, duration);
									}
								} else if (!actor->IsPlayerRef() && npcEffects) {
									if (impact.kind == FootEvent::JumpLand) { // let Rumble Manager handle it.
										DoJumpingRumble(actor, tremor * calamity, 0.03f, node->name, duration);
									} else {
										ApplyShakeAtPoint(actor, tremor * calamity, node->world.translate, duration);
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
