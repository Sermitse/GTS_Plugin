#include "Managers/Audio/Stomps.hpp"
#include "Managers/Audio/Footstep.hpp"

#include "Config/Config.hpp"

#include "Managers/Audio/AudioObtainer.hpp"
#include "Managers/Audio/AudioParams.hpp"
#include "Managers/Highheel.hpp"

using namespace GTS;

namespace GTS {

	void StompManager::PlayNewOrOldStomps(Actor* giant, float modifier, FootEvent foot_kind, std::string_view find_foot, bool Strong) {
		const bool UseOtherHeelSet = Config::Audio.bUseOtherHighHeelSet;

		if (UseOtherHeelSet && HighHeelManager::IsWearingHH(giant)) {
			PlayStompSounds(giant, modifier, find_foot, foot_kind, get_visual_scale(giant), Strong); // Play stomp sounds made by TimKroyer
		} else {
			DoFootstepSound(giant, modifier, foot_kind, find_foot); // Just treat it like footsteps
		}
	}

	void StompManager::PlayStompSounds(Actor* giant, float modifier, std::string_view find_foot, FootEvent foot_kind, float scale, bool Strong) {
		//https://www.desmos.com/calculator/wh0vwgljfl
		GTS_PROFILE_SCOPE("StompManager: PlayHighHeelSounds");
		if (giant) {
			modifier *= Volume_Multiply_Function(giant, foot_kind);
			if (giant->formID == 0x14 && HasSMT(giant)) {
				scale *= 2.5f;
			}
			FootStepManager::GetSingleton().DoStompSounds(giant, modifier, find_node(giant, find_foot), foot_kind, scale, Strong);
		}
	}

	float StompManager::Volume_Multiply_Function(Actor* actor, FootEvent Kind) {
		float modifier = 1.0f;
		if (actor) {
			if (actor->AsActorState()->IsSprinting()) { // Sprinting makes you sound bigger
				modifier *= 1.10f;
			}
			if (actor->AsActorState()->IsWalking()) {
				modifier *= 0.70f; // Walking makes you sound quieter
			}
			if (actor->IsSneaking()) {
				modifier *= 0.70f; // Sneaking makes you sound quieter
			}

			if (Kind == FootEvent::JumpLand) {
				modifier *= 1.2f; // Jumping makes you sound bigger
			}
			modifier *= 1.0f + (Potion_GetMightBonus(actor) * 0.33f);
		}
		return modifier;
	}
}