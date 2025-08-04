#include "Managers/Animation/Vore_Standing.hpp"

#include "config/Config.hpp"

#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Controllers/VoreController.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Rumble.hpp"

using namespace GTS;


//                Sorted by order and timings
//GTSvore_sit_start                         //Start sit down and shake
//GTSvore_impactLS                          //Silent left feet impact
//GTSvore_sit_end                           //Sit end, stop shake
//GTSvore_hand_extend                       //Hand starts to move in space
//GTSvore_hand_grab                         //Hand grabs someoone
//GTSvore_attachactor_AnimObject_A          //Same as above
//GTSvore_bringactor_start                  //Hand starts to move victim in space
//GTSvore_open_mouth                        //Open mouth
//GTSvore_bringactor_end                    //Drop actor into mouth
//GTSvore_swallow                           //Swallow actor
//GTSvore_swallow_sound                     //Pretty much the same
//GTSvore_close_mouth                       //Close mouth
//GTSvore_handR_reposition_S                //Right hand starts to return to normal position
//GTSvore_handL_reposition_S                //Same but for left hand
//GTSvore_handR_reposition_E                //Right hand returned to normal position
//GTSvore_handL_reposition_E                //Same but for left hand
//GTSvore_eat_actor                         //Kill and eat actor completely
//GTSvore_detachactor_AnimObject_A          //Actor is no longer attached to AnimObjectA
//GTSvore_standup_start                     //Return to normal stance
//GTSvore_impactRS                          //Right feet collides with the ground
//GTSvore_standup_end                       //Exit animation

namespace {

	constexpr std::string_view RNode = "NPC R Foot [Rft ]";
	constexpr std::string_view LNode = "NPC L Foot [Lft ]";

	const std::vector<std::string_view> RHAND_RUMBLE_NODES = { // used for hand rumble
		"NPC R UpperarmTwist1 [RUt1]",
		"NPC R UpperarmTwist2 [RUt2]",
		"NPC R Forearm [RLar]",
		"NPC R ForearmTwist2 [RLt2]",
		"NPC R ForearmTwist1 [RLt1]",
		"NPC R Hand [RHnd]",
	};

	const std::vector<std::string_view> LHAND_RUMBLE_NODES = { // used for hand rumble
		"NPC L UpperarmTwist1 [LUt1]",
		"NPC L UpperarmTwist2 [LUt2]",
		"NPC L Forearm [LLar]",
		"NPC L ForearmTwist2 [LLt2]",
		"NPC L ForearmTwist1 [LLt1]",
		"NPC L Hand [LHnd]",
	};

	const std::vector<std::string_view> BODY_RUMBLE_NODES = { // used for body rumble
		"NPC COM [COM ]",
		"NPC L Foot [Lft ]",
		"NPC R Foot [Rft ]",
		"NPC L Toe0 [LToe]",
		"NPC R Toe0 [RToe]",
		"NPC L Calf [LClf]",
		"NPC R Calf [RClf]",
		"NPC L PreRearCalf",
		"NPC R PreRearCalf",
		"NPC L FrontThigh",
		"NPC R FrontThigh",
		"NPC R RearCalf [RrClf]",
		"NPC L RearCalf [RrClf]",
	};



	void StartBodyRumble(std::string_view tag, Actor& actor, float power, float halflife, bool once) {
		for (auto& node_name: BODY_RUMBLE_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			if (!once) {
				Rumbling::Start(rumbleName, &actor, power,  halflife, node_name);
			} else {
				Rumbling::Once(rumbleName, &actor, power, halflife, node_name, 0.0f);
			}
		}
	}

	void StartRHandRumble(std::string_view tag, Actor& actor, float power, float halflife) {
		for (auto& node_name: RHAND_RUMBLE_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			Rumbling::Start(rumbleName, &actor, power,  halflife, node_name);
		}
	}

	void StartLHandRumble(std::string_view tag, Actor& actor, float power, float halflife) {
		for (auto& node_name: LHAND_RUMBLE_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			Rumbling::Start(rumbleName, &actor, power,  halflife, node_name);
		}
	}

	void StopRHandRumble(std::string_view tag, Actor& actor) {
		for (auto& node_name: RHAND_RUMBLE_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			Rumbling::Stop(rumbleName, &actor);
		}
	}
	void StopLHandRumble(std::string_view tag, Actor& actor) {
		for (auto& node_name: RHAND_RUMBLE_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			Rumbling::Stop(rumbleName, &actor);
		}
	}

	void GTSvore_sit_start(AnimationEventData& data) {
		auto giant = &data.giant;
		auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);
		for (auto& tiny: VoreData.GetVories()) {
			AllowToBeCrushed(tiny, false);
			DisableCollisions(tiny, giant);
			SetBeingHeld(tiny, true);
		}
		const bool Freelook = Config::GetGameplay().ActionSettings.bVoreFreecam;

		if (Freelook && giant->formID == 0x14) {
			EnableFreeCamera();
		} else {
			ManageCamera(giant, true, CameraTracking::Hand_Right);
		}

		StartBodyRumble("BodyRumble", data.giant, 0.15f, 0.10f, false);

		Task_HighHeel_SyncVoreAnim(giant);

	}

	void GTSvore_impactLS(AnimationEventData& data) {
		auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);
		for (auto& tiny: VoreData.GetVories()) {
			tiny->NotifyAnimationGraph("GTS_EnterFear");
		}
		VoreData.AllowToBeVored(false);

		
		float shake_power = Rumble_Vore_Stomp_Light * GetHighHeelsBonusDamage(&data.giant, true);
		if (HasSMT(&data.giant)) {
			shake_power *= 2.0f;
		}
		Rumbling::Once("StompLS", &data.giant, shake_power, 0.05f, LNode,  0.0f);
		
		DoFootstepSound(&data.giant, 0.90f, FootEvent::Left, LNode);
		DoDustExplosion(&data.giant, 0.90f, FootEvent::Left, LNode);
		DoDamageEffect(&data.giant, Damage_Vore_Standing_Footstep, Radius_Vore_Standing_Footstep, 30, 0.25f, FootEvent::Left, 1.0f, DamageSource::CrushedLeft);
		DoLaunch(&data.giant, 0.50f, 1.20f, FootEvent::Left);
		
	}

	void GTSvore_sit_end(AnimationEventData& data) {
		Rumbling::Stop("BodyRumble", &data.giant);
		AdjustFacialExpression(&data.giant, 2, 1.0f, CharEmotionType::Expression); // smile (expression)
	}

	void GTSvore_hand_extend(AnimationEventData& data) {
		StartRHandRumble("HandR", data.giant, 0.25f, 0.15f);
		auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);
		for (auto& tiny: VoreData.GetVories()) {
			tiny->NotifyAnimationGraph("GTS_ExitFear");
		}
	}

	void GTSvore_hand_grab(AnimationEventData& data) {
		auto giant = &data.giant;
		auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);
		VoreData.GrabAll();
		for (auto& tiny: VoreData.GetVories()) {
			tiny->NotifyAnimationGraph("JumpFall");
			Attacked(tiny, giant);
		}

		ManageCamera(giant, false, CameraTracking::Hand_Right);
		ManageCamera(giant, true, CameraTracking::VoreHand_Right);
		
		StopRHandRumble("HandR", data.giant);
	}

	void GTSvore_attachactor_AnimObject_A(AnimationEventData& data) {
	}

	void GTSvore_bringactor_start(AnimationEventData& data) {
		AdjustFacialExpression(&data.giant, 3, 0.8f, CharEmotionType::Phenome); // Smile a bit (Mouth)
		StartRHandRumble("HandR", data.giant, 0.2f, 0.175f);
	}

	void GTSvore_open_mouth(AnimationEventData& data) {
		auto giant = &data.giant;
		auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);
		Task_FacialEmotionTask_OpenMouth(giant, 0.75f, "StandingVoreOpenMouth");

		for (auto& tiny: VoreData.GetVories()) {
			VoreController::GetSingleton().ShrinkOverTime(giant, tiny);
		}
	}

	void GTSvore_bringactor_end(AnimationEventData& data) {
		auto giant = &data.giant;
		auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);
		StopRHandRumble("HandR", data.giant);
		VoreData.AllowToBeVored(true);
		for (auto& tiny: VoreData.GetVories()) {
			AllowToBeCrushed(tiny, true);
		}
	}


	void GTSvore_swallow(AnimationEventData& data) {
		auto giant = &data.giant;
		auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);
		VoreData.EnableMouthShrinkZone(true);
		if (AllowDevourment()) {
			for (auto& tiny: VoreData.GetVories()) {
				CallDevourment(giant, tiny);
			} 
		} else {
			Runtime::PlaySoundAtNode("GTSSoundSwallow", giant, 1.0f, "NPC Head [Head]"); // Play sound
			VoreData.Swallow();
		}
	}

	void GTSvore_swallow_sound(AnimationEventData& data) {
		AdjustFacialExpression(&data.giant, 3, 0.0f, CharEmotionType::Phenome); // Remove smile (Mouth)
	}

	void GTSvore_close_mouth(AnimationEventData& data) {
		auto giant = &data.giant;
		auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);

		VoreData.EnableMouthShrinkZone(false);

		std::string name_1 = std::format("Phenome_{}_{}_{}", giant->formID, 0, 1.0f);
		std::string name_2 = std::format("Phenome_{}_{}_{}", giant->formID, 1, 0.5f);

		TaskManager::Cancel(name_1);
		TaskManager::Cancel(name_2);

		for (auto& tiny: VoreData.GetVories()) {
			if (tiny->formID == 0x14) {
				PlayerCamera::GetSingleton()->cameraTarget = giant->CreateRefHandle();
			}
		}
	}

	void GTSvore_handR_reposition_S(AnimationEventData& data) {
		
		auto giant = &data.giant;
		/*if (!AllowDevourment()) {
			
		}*/
		AdjustFacialExpression(giant, 0, 0.0f, CharEmotionType::Modifier); // blink L
		AdjustFacialExpression(giant, 1, 0.0f, CharEmotionType::Modifier); // blink R
		StartRHandRumble("HandR", data.giant, 0.20f, 0.15f);
	}

	void GTSvore_handL_reposition_S(AnimationEventData& data) {
		StartLHandRumble("HandL", data.giant, 0.20f, 0.15f);
	}

	void GTSvore_handR_reposition_E(AnimationEventData& data) {
		StopRHandRumble("HandR", data.giant);
	}

	void GTSvore_handL_reposition_E(AnimationEventData& data) {
		StopLHandRumble("HandL", data.giant);
	}

	void GTSvore_eat_actor(AnimationEventData& data) {
		auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);
		AdjustFacialExpression(&data.giant, 2, 0.0f, CharEmotionType::Expression); // Remove smile
		VoreData.KillAll();
	}

	void GTSvore_detachactor_AnimObject_A(AnimationEventData& data) {
	}

	void GTSvore_standup_start(AnimationEventData& data) {
		auto giant = &data.giant;
		StartBodyRumble("BodyRumble", data.giant, 0.15f, 0.10f, false);
		
		ManageCamera(giant, false, CameraTracking::Hand_Right);
		ManageCamera(giant, false, CameraTracking::VoreHand_Right);

	}

	void GTSvore_impactRS(AnimationEventData& data) {
		Rumbling::Once("StompRS", &data.giant, 0.95f, 0.05f, RNode, 0.0f);
		float perk = GetPerkBonus_Basics(&data.giant);
		DoFootstepSound(&data.giant, 0.90f, FootEvent::Right, RNode);
		DoDustExplosion(&data.giant, 0.90f, FootEvent::Right, RNode);
		DoDamageEffect(&data.giant, Damage_Vore_Standing_Footstep, Radius_Vore_Standing_Footstep, 30, 0.25f, FootEvent::Right, 1.0f, DamageSource::CrushedRight);
		DoLaunch(&data.giant, 0.70f * perk, 1.20f * data.animSpeed, FootEvent::Right);
	}

	void GTSvore_standup_end(AnimationEventData& data) {
		auto giant = &data.giant;
		auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);
		VoreData.ReleaseAll();
		if (Config::GetGameplay().ActionSettings.bVoreFreecam && giant->formID == 0x14) {
			EnableFreeCamera();
		}
		Rumbling::Stop("BodyRumble", &data.giant);

		Utils_UpdateHighHeelBlend(giant, true);
	}
}


namespace GTS
{
	void Animation_VoreStanding::RegisterEvents() {
		AnimationManager::RegisterEvent("GTSvore_attachactor_AnimObject_A", "Vore", GTSvore_attachactor_AnimObject_A);
		AnimationManager::RegisterEvent("GTSvore_detachactor_AnimObject_A", "Vore", GTSvore_detachactor_AnimObject_A);
		AnimationManager::RegisterEvent("GTSvore_handR_reposition_S", "Vore", GTSvore_handR_reposition_S);
		AnimationManager::RegisterEvent("GTSvore_handL_reposition_S", "Vore", GTSvore_handL_reposition_S);
		AnimationManager::RegisterEvent("GTSvore_handR_reposition_E", "Vore", GTSvore_handR_reposition_E);
		AnimationManager::RegisterEvent("GTSvore_handL_reposition_E", "Vore", GTSvore_handL_reposition_E);
		AnimationManager::RegisterEvent("GTSvore_bringactor_start", "Vore", GTSvore_bringactor_start);
		AnimationManager::RegisterEvent("GTSvore_bringactor_end", "Vore", GTSvore_bringactor_end);
		AnimationManager::RegisterEvent("GTSvore_swallow_sound", "Vore", GTSvore_swallow_sound);
		AnimationManager::RegisterEvent("GTSvore_standup_start", "Vore", GTSvore_standup_start);
		AnimationManager::RegisterEvent("GTSvore_hand_extend", "Vore", GTSvore_hand_extend);
		AnimationManager::RegisterEvent("GTSvore_close_mouth", "Vore", GTSvore_close_mouth);
		AnimationManager::RegisterEvent("GTSvore_standup_end", "Vore", GTSvore_standup_end);
		AnimationManager::RegisterEvent("GTSvore_open_mouth", "Vore", GTSvore_open_mouth);
		AnimationManager::RegisterEvent("GTSvore_hand_grab", "Vore", GTSvore_hand_grab);
		AnimationManager::RegisterEvent("GTSvore_sit_start", "Vore", GTSvore_sit_start);
		AnimationManager::RegisterEvent("GTSvore_eat_actor", "Vore", GTSvore_eat_actor);
		AnimationManager::RegisterEvent("GTSvore_impactRS", "Vore", GTSvore_impactRS);
		AnimationManager::RegisterEvent("GTSvore_impactLS", "Vore", GTSvore_impactLS);
		AnimationManager::RegisterEvent("GTSvore_sit_end", "Vore", GTSvore_sit_end);
		AnimationManager::RegisterEvent("GTSvore_swallow", "Vore", GTSvore_swallow);
	}

	void Animation_VoreStanding::RegisterTriggers() {
		AnimationManager::RegisterTrigger("StartVore", "Vore", "GTSBEH_StartVore");
	}
}
