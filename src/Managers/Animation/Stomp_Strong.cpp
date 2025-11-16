#include "Managers/Animation/Stomp_Strong.hpp"
#include "Managers/Animation/Stomp_Under.hpp"
#include "Managers/Animation/AnimationManager.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Audio/Footstep.hpp"
#include "Managers/Audio/Stomps.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Managers/Rumble.hpp"

#include "Utils/Actions/InputConditions.hpp"

#include "Managers/Perks/PerkHandler.hpp"

using namespace GTS;

namespace {

	const std::vector<std::string_view> R_LEG_RUMBLE_NODES = { 
		"NPC L Toe0 [LToe]",
		"NPC L Calf [LClf]",
		"NPC L PreRearCalf",
		"NPC L FrontThigh",
		"NPC L RearCalf [RrClf]",
	};
	const std::vector<std::string_view> L_LEG_RUMBLE_NODES = { 
		"NPC R Toe0 [RToe]",
		"NPC R Calf [RClf]",
		"NPC R PreRearCalf",
		"NPC R FrontThigh",
		"NPC R RearCalf [RrClf]",
	};

	const std::string_view RNode = "NPC R Foot [Rft ]";
	const std::string_view LNode = "NPC L Foot [Lft ]";

	void DoStompOrUnderStomp(Actor* player, const std::string_view name) {
		float WasteStamina = 70.0f * GetWasteMult(player);

		std::string_view message = "You're too tired to perform heavy stomp";

		if (player->IsSneaking()) {
			if (AnimationUnderStomp::ShouldStompUnder(player)) {
				if (!IsCrawling(player)) {
					message = "You're too tired to perform sneak butt crush";
					WasteStamina *= 1.8f;
				} else {
					message = "You're too tired to perform sneak breast crush";
					WasteStamina *= 2.5f;
				}	
			}
		}

		if (GetAV(player, ActorValue::kStamina) > WasteStamina) {
			AnimationManager::StartAnim(name, player);
		} else {
			NotifyWithSound(player, message);
		}
	}

	void StartLegRumbling(std::string_view tag, Actor& actor, float power, float halflife, bool right) {
		// Disabled because it messed up normal shake life-time, overriding it with the new one
		if (!right) {
			for (auto& node_name: L_LEG_RUMBLE_NODES) {
				std::string rumbleName = std::format("{}{}", tag, node_name);
				Rumbling::Start(rumbleName, &actor, power,  halflife, node_name);
			}
		} else {
			for (auto& node_name: R_LEG_RUMBLE_NODES) {
				std::string rumbleName = std::format("{}{}", tag, node_name);
				Rumbling::Start(rumbleName, &actor, power,  halflife, node_name);
			}
		}
	}

	void StopLegRumbling(std::string_view tag, Actor& actor, bool right) {
		if (!right) {
			for (auto& node_name: L_LEG_RUMBLE_NODES) {
				std::string rumbleName = std::format("{}{}", tag, node_name);
				Rumbling::Stop(rumbleName, &actor);
			}
		} else {
			for (auto& node_name: R_LEG_RUMBLE_NODES) {
				std::string rumbleName = std::format("{}{}", tag, node_name);
				Rumbling::Stop(rumbleName, &actor);
			}
		}
	}

	void DoImpactRumble(Actor* giant, std::string_view node, std::string_view name, float mult = 1.0f) {
		float shake_power = Rumble_Stomp_Strong;
		float smt = HasSMT(giant) ? 1.5f : 1.0f;
		
		smt *= GetHighHeelsBonusDamage(giant, true);
		Rumbling::Once(name, giant, shake_power * smt * mult, 0.0f, node, 1.25f);
	}

	void StrongStomp_DoEverything(Actor* giant, float animSpeed, bool right, FootEvent Event, DamageSource Source, std::string_view Node, std::string_view rumble) {
		float perk = GetPerkBonus_Basics(giant);
		float SMT = 1.0f;
		float damage = 1.0f;
		if (HasSMT(giant)) {
			SMT = 1.85f; // Larger Dust
			damage = 1.25f;
		}

		std::string taskname = std::format("StrongStompAttack_{}", giant->formID);
		ActorHandle giantHandle = giant->CreateRefHandle();

		double Start = Time::WorldTimeElapsed();
		
		TaskManager::RunFor(taskname, 1.0f, [=](auto& update){ // Needed because anim has wrong timing
			if (!giantHandle) {
				return false;
			}

			double Finish = Time::WorldTimeElapsed();
			auto giantref = giantHandle.get().get();

			if (Finish - Start > 0.07) { 
				float Augment = PerkHandler::Perks_Cataclysmic_EmpowerStomp(giantref);
				bool GotStacks = PerkHandler::Perks_Cataclysmic_HasStacks(giantref);

				DoDamageEffect(giantref, Damage_Stomp_Strong * damage * perk * Augment, Radius_Stomp_Strong, 5, 0.35f, Event, 1.0f, Source);
				DoImpactRumble(giantref, Node, rumble);

				if (!GotStacks) { 
					DoDustExplosion(giantref, 1.33f * (SMT + (animSpeed * 0.05f)), Event, Node);
				} else {
					for (auto exp: {1.0f, 0.75f, 0.5f}) { // Multi-explosion, show that it's strong
						DoDustExplosion(giantref, 1.33f * (SMT + (animSpeed * 0.05f)) * Augment * exp, Event, Node);
					}
				}

				DrainStamina(giantref, "StaminaDrain_StrongStomp", Runtime::PERK.GTSPerkDestructionBasics, false, 3.4f);

				StompManager::PlayNewOrOldStomps(giantref, SMT + (animSpeed/10), Event, Node, true);

				LaunchTask(giantref, 1.05f * perk * Augment, (3.6f + animSpeed/2) * Augment, Event);

				FootStepManager::DoStrongSounds(giantref, 1.15f + animSpeed/20, Node);
				FootStepManager::PlayVanillaFootstepSounds(giantref, right);

				SetBusyFoot(giantref, BusyFoot::None);

				return false;
			}
			return true;
		});
		
	}

	///////////////////////////////////////////////////////////////////////
	//      EVENTS
	//////////////////////////////////////////////////////////////////////

	void GTS_StrongStomp_Start(AnimationEventData& data) {
		data.stage = 1;
		data.animSpeed = 1.35f;
	}

	void GTS_StrongStomp_LR_Start(AnimationEventData& data) {
		auto giant = &data.giant;
		data.stage = 1;
		data.canEditAnimSpeed = true;
		if (data.giant.formID != 0x14) {
			data.animSpeed += GetRandomBoost()/3;
		}
		ManageCamera(giant, true, CameraTracking::R_Foot);
		DrainStamina(&data.giant, "StaminaDrain_StrongStomp", Runtime::PERK.GTSPerkDestructionBasics, true, 3.4f);

		SetBusyFoot(&data.giant, BusyFoot::RightFoot);
		PerkHandler::Perks_Cataclysmic_BuffStompSpeed(data, false);
	}

	void GTS_StrongStomp_LL_Start(AnimationEventData& data) {
		auto giant = &data.giant;
		data.stage = 1;
		data.canEditAnimSpeed = true;
		if (data.giant.formID != 0x14) {
			data.animSpeed += GetRandomBoost()/3;
		}
		ManageCamera(giant, true, CameraTracking::L_Foot);
		DrainStamina(&data.giant, "StaminaDrain_StrongStomp", Runtime::PERK.GTSPerkDestructionBasics, true, 3.4f);

		SetBusyFoot(&data.giant, BusyFoot::LeftFoot);

		PerkHandler::Perks_Cataclysmic_BuffStompSpeed(data, false);
	}

	void GTS_StrongStomp_LR_Middle(AnimationEventData& data) {
		data.animSpeed = 1.55f;
		if (data.giant.formID != 0x14) {
			data.animSpeed = 1.55f + GetRandomBoost();
		}
		PerkHandler::Perks_Cataclysmic_BuffStompSpeed(data, false);
	}
	void GTS_StrongStomp_LL_Middle(AnimationEventData& data) {
		data.animSpeed = 1.55f;
		if (data.giant.formID != 0x14) {
			data.animSpeed = 1.55f + GetRandomBoost();
		}
		PerkHandler::Perks_Cataclysmic_BuffStompSpeed(data, false);
	}
	void GTS_StrongStomp_LR_End(AnimationEventData& data) {
		StopLegRumbling("StrongStompR", data.giant, true);
	}
	void GTS_StrongStomp_LL_End(AnimationEventData& data) {
		StopLegRumbling("StrongStompL", data.giant, false);
	}

	void GTS_StrongStomp_ImpactR(AnimationEventData& data) {
		StrongStomp_DoEverything(&data.giant, data.animSpeed, true, FootEvent::Right, DamageSource::CrushedRight, RNode, "HeavyStompR");
		PerkHandler::Perks_Cataclysmic_BuffStompSpeed(data, true);
	}
	void GTS_StrongStomp_ImpactL(AnimationEventData& data) {
		StrongStomp_DoEverything(&data.giant, data.animSpeed, false, FootEvent::Left, DamageSource::CrushedLeft, LNode, "HeavyStompL");
		PerkHandler::Perks_Cataclysmic_BuffStompSpeed(data, true);
	}

	void GTS_StrongStomp_ReturnRL_Start(AnimationEventData& data) {StartLegRumbling("StrongStompR", data.giant, 0.25f, 0.10f, true);}
	void GTS_StrongStomp_ReturnLL_Start(AnimationEventData& data) {StartLegRumbling("StrongStompL", data.giant, 0.25f, 0.10f, false);}

	void GTS_StrongStomp_ReturnRL_End(AnimationEventData& data) {
		StopLegRumbling("StrongStompR", data.giant, true);
		ManageCamera(&data.giant, false, CameraTracking::R_Foot);
	}
	void GTS_StrongStomp_ReturnLL_End(AnimationEventData& data) {
		StopLegRumbling("StrongStompL", data.giant, false);
		ManageCamera(&data.giant, false, CameraTracking::L_Foot);
	}
	void GTS_StrongStomp_End(AnimationEventData& data) {
		StopLegRumbling("StrongStompR", data.giant, true);
		StopLegRumbling("StrongStompL", data.giant, false);
	}


	void GTS_Next(AnimationEventData& data) {
		Rumbling::Stop("StompR", &data.giant);
	}

	void GTSBEH_Exit(AnimationEventData& data) {
		auto giant = &data.giant;
		ResetCameraTracking(giant);

		Rumbling::Stop("StompR", &data.giant);
	}

	void RightStrongStompEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		bool UnderStomp = AnimationUnderStomp::ShouldStompUnder(player);
		const std::string_view StompType = UnderStomp ? "UnderStompStrongRight" : "StrongStompRight";
		DoStompOrUnderStomp(player, StompType);
	}

	void LeftStrongStompEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		bool UnderStomp = AnimationUnderStomp::ShouldStompUnder(player);
		const std::string_view StompType = UnderStomp ? "UnderStompStrongLeft" : "StrongStompLeft";
		DoStompOrUnderStomp(player, StompType);
	}
}

namespace GTS
{
	void AnimationStrongStomp::RegisterEvents() {
		AnimationManager::RegisterEvent("GTS_StrongStomp_Start", "StrongStomp", GTS_StrongStomp_Start);
		AnimationManager::RegisterEvent("GTS_StrongStomp_LR_Start", "StrongStomp", GTS_StrongStomp_LR_Start);
		AnimationManager::RegisterEvent("GTS_StrongStomp_LL_Start", "StrongStomp", GTS_StrongStomp_LL_Start);
		AnimationManager::RegisterEvent("GTS_StrongStomp_LR_Middle", "StrongStomp", GTS_StrongStomp_LR_Middle);
		AnimationManager::RegisterEvent("GTS_StrongStomp_LL_Middle", "StrongStomp", GTS_StrongStomp_LL_Middle);
		AnimationManager::RegisterEvent("GTS_StrongStomp_LR_End", "StrongStomp", GTS_StrongStomp_LR_End);
		AnimationManager::RegisterEvent("GTS_StrongStomp_LL_End", "StrongStomp", GTS_StrongStomp_LL_End);
		AnimationManager::RegisterEvent("GTS_StrongStomp_ImpactR", "StrongStomp", GTS_StrongStomp_ImpactR);
		AnimationManager::RegisterEvent("GTS_StrongStomp_ImpactL", "StrongStomp", GTS_StrongStomp_ImpactL);
		AnimationManager::RegisterEvent("GTS_StrongStomp_ReturnRL_Start", "StrongStomp", GTS_StrongStomp_ReturnRL_Start);
		AnimationManager::RegisterEvent("GTS_StrongStomp_ReturnLL_Start", "StrongStomp", GTS_StrongStomp_ReturnLL_Start);
		AnimationManager::RegisterEvent("GTS_StrongStomp_ReturnRL_End", "StrongStomp", GTS_StrongStomp_ReturnRL_End);
		AnimationManager::RegisterEvent("GTS_StrongStomp_ReturnLL_End", "StrongStomp", GTS_StrongStomp_ReturnLL_End);
		AnimationManager::RegisterEvent("GTS_StrongStomp_End", "StrongStomp", GTS_StrongStomp_End);
		AnimationManager::RegisterEvent("GTS_Next", "StrongStomp", GTS_Next);
		AnimationManager::RegisterEvent("GTSBEH_Exit", "StrongStomp", GTSBEH_Exit);

		InputManager::RegisterInputEvent("RightStomp_Strong", RightStrongStompEvent, StompCondition);
		InputManager::RegisterInputEvent("LeftStomp_Strong", LeftStrongStompEvent, StompCondition);
	}

	void AnimationStrongStomp::RegisterTriggers() {
		AnimationManager::RegisterTrigger("StrongStompRight", "Stomp", "GTSBeh_StrongStomp_StartRight");
		AnimationManager::RegisterTrigger("StrongStompLeft", "Stomp", "GTSBeh_StrongStomp_StartLeft");
	}
}
