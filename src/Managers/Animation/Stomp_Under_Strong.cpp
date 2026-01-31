#include "Managers/Animation/Stomp_Under_Strong.hpp"
#include "Managers/Animation/AnimationManager.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Audio/Footstep.hpp"
#include "Managers/Audio/Stomps.hpp"
#include "Managers/Rumble.hpp"

#include "Managers/Perks/PerkHandler.hpp"

using namespace GTS;

namespace {

	constexpr std::string_view RNode = "NPC R Foot [Rft ]";
	constexpr std::string_view LNode = "NPC L Foot [Lft ]";

    void DoImpactRumble(Actor* giant, std::string_view node, std::string_view name, float Mult = 1.0f) {
		float shake_power = Rumble_Stomp_Under_Strong;
		float smt = HasSMT(giant) ? 1.5f : 1.0f;
		smt *= GetHighHeelsBonusDamage(giant, true);
		Rumbling::Once(name, giant, shake_power * smt * Mult, 0.0f, node, 1.25f);
	}

    void UnderStomp_DoEverything(Actor* giant, float animSpeed, bool right, FootEvent Event, DamageSource Source, std::string_view Node, std::string_view rumble) {
		float perk = GetPerkBonus_Basics(giant);
		float SMT = 1.0f;
		float damage = 1.0f;
		if (HasSMT(giant)) {
			SMT = 1.75f; // Larger Dust
			damage = 1.25f;
		}
		std::string taskname = std::format("StrongUnderStomp_{}", giant->formID);
		ActorHandle giantHandle = giant->CreateRefHandle();

		double Start = Time::WorldTimeElapsed();
		
		TaskManager::RunFor(taskname, 1.0f, [=](auto& update){ // Needed because anim has wrong timing
			if (!giantHandle) {
				return false;
			}

			double Finish = Time::WorldTimeElapsed();
			auto giantref = giantHandle.get().get();

			if (Finish - Start > 0.05) { 
				float Augment = PerkHandler::Perks_Cataclysmic_EmpowerStomp(giantref);
				bool GotStacks = PerkHandler::Perks_Cataclysmic_HasStacks(giantref);

				DoDamageEffect(giantref, Damage_Stomp_Under_Strong * damage * perk * Augment, Radius_Stomp_Strong, 8, 0.30f, Event, 1.0f, Source, false);
				DoImpactRumble(giantref, Node, rumble);

				if (!GotStacks) { 
					DoDustExplosion(giantref, 1.0f * SMT * Augment, Event, Node);
				} else {
					for (auto exp: {1.0f, 0.75f, 0.5f}) { // Multi-explosion, show that it's strong
						DoDustExplosion(giantref, 1.0f * SMT * Augment * exp, Event, Node);
					}
				}

				DrainStamina(giantref,"StaminaDrain_StrongStomp", Runtime::PERK.GTSPerkDestructionBasics, false, 5.1f); // 13.5 * 5.1

				StompManager::PlayNewOrOldStomps(giantref, SMT, Event, Node, true);

				LaunchTask(giantref, 1.05f * perk * Augment, 3.60f * Augment, Event);

				FootStepManager::DoStrongSounds(giantref, 1.10f + animSpeed/20, Node);
				FootStepManager::PlayVanillaFootstepSounds(giantref, right);

				SetBusyFoot(giantref, BusyFoot::None);
				
				return false;
			}
			return true;
		});
	}

    void GTS_UnderStomp_CamOn_StrongR(AnimationEventData& data) {
        DrainStamina(&data.giant, "StaminaDrain_StrongStomp", Runtime::PERK.GTSPerkDestructionBasics, true, 5.1f);
        ManageCamera(&data.giant, true, CameraTracking::R_Foot);
		SetBusyFoot(&data.giant, BusyFoot::RightFoot);

		PerkHandler::Perks_Cataclysmic_BuffStompSpeed(data, false);
    }

    void GTS_UnderStomp_CamOn_StrongL(AnimationEventData& data) {
        DrainStamina(&data.giant, "StaminaDrain_StrongStomp", Runtime::PERK.GTSPerkDestructionBasics, true, 5.1f);
        ManageCamera(&data.giant, true, CameraTracking::L_Foot);
		SetBusyFoot(&data.giant, BusyFoot::LeftFoot);

		PerkHandler::Perks_Cataclysmic_BuffStompSpeed(data, false);
    }

    void GTS_UnderStomp_CamOff_StrongR(AnimationEventData& data) {ManageCamera(&data.giant, false, CameraTracking::R_Foot);}
    void GTS_UnderStomp_CamOff_StrongL(AnimationEventData& data) {ManageCamera(&data.giant, false, CameraTracking::L_Foot);}

    void GTS_UnderStomp_Impact_StrongR(AnimationEventData& data) {
		UnderStomp_DoEverything(&data.giant, data.animSpeed, true, FootEvent::Right, DamageSource::CrushedRight, RNode, "HeavyStompR");
		PerkHandler::Perks_Cataclysmic_BuffStompSpeed(data, true);
	}
	void GTS_UnderStomp_Impact_StrongL(AnimationEventData& data) {
		UnderStomp_DoEverything(&data.giant, data.animSpeed, false, FootEvent::Left, DamageSource::CrushedLeft, LNode, "HeavyStompL");
		PerkHandler::Perks_Cataclysmic_BuffStompSpeed(data, true);
	}
}
namespace GTS {
    void AnimationUnderStompStrong::RegisterEvents() {
        AnimationManager::RegisterEvent("GTS_UnderStomp_CamOn_StrongR", "UnderStomp", GTS_UnderStomp_CamOn_StrongR);
		AnimationManager::RegisterEvent("GTS_UnderStomp_CamOn_StrongL", "UnderStomp", GTS_UnderStomp_CamOn_StrongL);

        AnimationManager::RegisterEvent("GTS_UnderStomp_CamOff_StrongR", "UnderStomp", GTS_UnderStomp_CamOff_StrongR);
		AnimationManager::RegisterEvent("GTS_UnderStomp_CamOff_StrongL", "UnderStomp", GTS_UnderStomp_CamOff_StrongL);
        

		AnimationManager::RegisterEvent("GTS_UnderStomp_Impact_StrongR", "UnderStomp", GTS_UnderStomp_Impact_StrongR);
		AnimationManager::RegisterEvent("GTS_UnderStomp_Impact_StrongL", "UnderStomp", GTS_UnderStomp_Impact_StrongL);
	}

	void AnimationUnderStompStrong::RegisterTriggers() {
		AnimationManager::RegisterTrigger("UnderStompStrongRight", "Stomp", "GTSBeh_UnderStomp_Start_StrongR");
		AnimationManager::RegisterTrigger("UnderStompStrongLeft", "Stomp", "GTSBeh_UnderStomp_Start_StrongL");
	}
}