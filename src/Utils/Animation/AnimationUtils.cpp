#include "Config/Config.hpp"

namespace GTS {

	void PlayAnimation(Actor* actor, std::string_view animName) {
		actor->NotifyAnimationGraph(animName);
	}

	float GetAnimationSlowdown(Actor* giant) {

		if (giant) {

			auto& Gen = Config::General;
			auto& Adv = Config::Advanced;

			if (Gen.bDynamicAnimspeed) {

				if (giant->AsActorState()->GetSitSleepState() != SIT_SLEEP_STATE::kNormal) {
					return 1.0f; // For some reason makes furniture angles funny if there's anim slowdown. So we prevent that
				}

				const auto SpeedVars = Adv.fAnimSpeedFormula;

				const SoftPotential Speed = {
					SpeedVars[0],
					SpeedVars[1],
					SpeedVars[2],
					SpeedVars[3],
					SpeedVars[4]
				};

				float scale = get_visual_scale(giant);
				float speedmultcalc = soft_core(scale, Speed);
				speedmultcalc = std::clamp(speedmultcalc, Adv.fAnimspeedLowestBoundAllowed, 1.0f);

				if (IsGtsBusy(giant) && Adv.bGTSAnimsFullSpeed) {
					return 1.0f;
				}
				else if (giant->formID == 0x14) {
					return Adv.fAnimSpeedAdjMultPlayer * speedmultcalc;
				}
				else if (IsTeammate(giant)) {
					return Adv.fAnimSpeedAdjMultTeammate * speedmultcalc;
				}

				return speedmultcalc;
			}
		}
		return 1.0f;
	}

}
