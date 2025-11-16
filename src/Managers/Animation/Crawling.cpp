#include "Managers/Animation/Crawling.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Grab.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CrawlUtils.hpp"

#include "Managers/Damage/LaunchObject.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Managers/GtsSizeManager.hpp"

#include "Utils/Actions/InputConditions.hpp"

using namespace GTS;

namespace {

	void EnableHandTracking(Actor* giant, CrawlEvent kind, bool enable) {
		if (AllowCameraTracking()) {
			auto& sizemanager = SizeManager::GetSingleton();
			if (kind == CrawlEvent::RightHand) {
				sizemanager.SetTrackedBone(giant, enable, CameraTracking::Hand_Right);
			} else if (kind == CrawlEvent::LeftHand) {
				sizemanager.SetTrackedBone(giant, enable, CameraTracking::Hand_Left);
			}
		}
	}

	void GTS_Crawl_Knee_Trans_Impact(AnimationEventData& data) {
		auto giant = &data.giant;
		float scale = get_visual_scale(giant);
		DoCrawlingFunctions(giant, scale, 2.6f, Damage_Crawl_KneeImpact_Drop, CrawlEvent::LeftKnee, "LeftKnee", 1.8f, Radius_Crawl_KneeImpact_Fall, 1.15f, DamageSource::KneeDropLeft);
		DoCrawlingFunctions(giant, scale, 2.6f, Damage_Crawl_KneeImpact_Drop, CrawlEvent::RightKnee, "RightKnee", 1.8f, Radius_Crawl_KneeImpact_Fall, 1.15f, DamageSource::KneeDropRight);
		//                                     launch power                                ^    ^ --- Size Damage Radius
		//                                                                             Launch       ^ -- crush threshold
		//                                                                             Radius
	}

	void GTS_Crawl_Hand_Trans_Impact(AnimationEventData& data) {
		auto giant = &data.giant;
		float scale = get_visual_scale(giant);
		DoCrawlingFunctions(giant, scale, 2.15f, Damage_Crawl_HandImpact_Drop, CrawlEvent::LeftHand, "LeftHand", 1.6f, Radius_Crawl_HandImpact_Fall, 1.15f, DamageSource::HandDropLeft);
		DoCrawlingFunctions(giant, scale, 2.15f, Damage_Crawl_HandImpact_Drop, CrawlEvent::RightHand, "RightHand", 1.6f, Radius_Crawl_HandImpact_Fall, 1.15f, DamageSource::HandDropRight);
	}

	void GTSCrawl_KneeImpact_L(AnimationEventData& data) {
		auto giant = &data.giant;
		float scale = get_visual_scale(giant);
		DoCrawlingFunctions(giant, scale, 1.85f, Damage_Crawl_KneeImpact, CrawlEvent::LeftKnee, "LeftKnee", 1.2f, Radius_Crawl_KneeImpact, 1.25f, DamageSource::KneeLeft);
		ApplyThighDamage(giant, false, false, Radius_ThighCrush_ButtCrush_Drop, Damage_Crawl_KneeImpact * 0.75f, 0.35f, 1.0f, 14, DamageSource::ThighCrushed);
	}
	void GTSCrawl_KneeImpact_R(AnimationEventData& data) {
		auto giant = &data.giant;
		float scale = get_visual_scale(giant);
		DoCrawlingFunctions(giant, scale, 1.85f, Damage_Crawl_KneeImpact, CrawlEvent::RightKnee, "RightKnee", 1.2f, Radius_Crawl_KneeImpact, 1.25f, DamageSource::KneeRight);
		ApplyThighDamage(giant, true, false, Radius_ThighCrush_ButtCrush_Drop, Damage_Crawl_KneeImpact * 0.75f, 0.35f, 1.0f, 14, DamageSource::ThighCrushed);
	}
	void GTSCrawl_HandImpact_L(AnimationEventData& data) {
		auto giant = &data.giant;
		float scale = get_visual_scale(giant);
		if (IsTransferingTiny(giant)) {
			return; // Prevent effects from left hand
		}
		DoCrawlingFunctions(giant, scale, 1.70f, Damage_Crawl_HandImpact, CrawlEvent::LeftHand, "LeftHand", 1.0f, Radius_Crawl_HandImpact, 1.25f, DamageSource::HandCrawlLeft);
	}
	void GTSCrawl_HandImpact_R(AnimationEventData& data) {
		auto giant = &data.giant;
		float scale = get_visual_scale(giant);
		DoCrawlingFunctions(giant, scale, 1.70f, Damage_Crawl_HandImpact, CrawlEvent::RightHand, "RightHand", 1.0f, Radius_Crawl_HandImpact, 1.25f, DamageSource::HandCrawlRight);
		//                                                                               ^    ^ --- Size Damage Radius
		//                                                                             Launch
		//                                                                             Radius
	}

	void GTSCrawl_Slam_Raise_Arm_R(AnimationEventData& data) {
		DrainStamina(&data.giant, "StaminaDrain_CrawlStomp", Runtime::PERK.GTSPerkDestructionBasics, true, 1.4f);
		EnableHandTracking(&data.giant, CrawlEvent::RightHand, true);
	}

	void GTSCrawl_Slam_Raise_Arm_L(AnimationEventData& data) {
		DrainStamina(&data.giant, "StaminaDrain_CrawlStomp", Runtime::PERK.GTSPerkDestructionBasics, true, 1.4f);
		EnableHandTracking(&data.giant, CrawlEvent::LeftHand, true);
	}
	void GTSCrawl_SlamStrong_Raise_Arm_R(AnimationEventData& data) {
		DrainStamina(&data.giant, "StaminaDrain_CrawlStompStrong", Runtime::PERK.GTSPerkDestructionBasics, true, 2.3f);
		EnableHandTracking(&data.giant, CrawlEvent::RightHand, true);
	}

	void GTSCrawl_SlamStrong_Raise_Arm_L(AnimationEventData& data) {
		DrainStamina(&data.giant, "StaminaDrain_CrawlStompStrong", Runtime::PERK.GTSPerkDestructionBasics, true, 2.3f);
		EnableHandTracking(&data.giant, CrawlEvent::LeftHand, true);
	}

	void GTSCrawl_Slam_Lower_Arm_R(AnimationEventData& data) {
	}
	void GTSCrawl_SlamStrong_Lower_Arm_R(AnimationEventData& data) {
	}
	void GTSCrawl_Slam_Lower_Arm_L(AnimationEventData& data) {
	}
	void GTSCrawl_SlamStrong_Lower_Arm_L(AnimationEventData& data) {
	}

	void GTSCrawl_Slam_Impact_R(AnimationEventData& data) {
		auto giant = &data.giant;
		float scale = get_visual_scale(giant);
		DoCrawlingFunctions(giant, scale, 1.4f, Damage_Crawl_HandSlam, CrawlEvent::RightHand, "RightHandRumble", 0.9f, Radius_Crawl_Slam, 1.15f, DamageSource::HandSlamRight);
		DrainStamina(&data.giant, "StaminaDrain_CrawlStomp", Runtime::PERK.GTSPerkDestructionBasics, false, 1.4f);
		DrainStamina(&data.giant, "StaminaDrain_CrawlStompStrong", Runtime::PERK.GTSPerkDestructionBasics, false, 2.3f);
	}
	void GTSCrawl_Slam_Impact_L(AnimationEventData& data) {
		auto giant = &data.giant;
		float scale = get_visual_scale(giant);
		DoCrawlingFunctions(giant, scale, 1.4f, Damage_Crawl_HandSlam, CrawlEvent::LeftHand, "LeftHandRumble", 0.9f, Radius_Crawl_Slam, 1.15f, DamageSource::HandSlamLeft);
		DrainStamina(&data.giant, "StaminaDrain_CrawlStomp", Runtime::PERK.GTSPerkDestructionBasics, false, 1.4f);
		DrainStamina(&data.giant, "StaminaDrain_CrawlStompStrong", Runtime::PERK.GTSPerkDestructionBasics, false, 2.3f);
		Grab::DamageActorInHand(giant, Damage_Crawl_HandSlam * 0.6f);
	}

	void GTSCrawl_SlamStrong_Impact_R(AnimationEventData& data) {
		auto giant = &data.giant;
		float scale = get_visual_scale(giant);
		DoCrawlingFunctions(giant, scale, 2.1f, Damage_Crawl_HandSlam_Strong, CrawlEvent::RightHand, "RightHandRumble", 1.2f, Radius_Crawl_Slam_Strong, 1.0f, DamageSource::HandSlamRight);
		DrainStamina(&data.giant, "StaminaDrain_CrawlStomp", Runtime::PERK.GTSPerkDestructionBasics, false, 1.4f);
		DrainStamina(&data.giant, "StaminaDrain_CrawlStompStrong", Runtime::PERK.GTSPerkDestructionBasics, false, 2.3f);
	}
	void GTSCrawl_SlamStrong_Impact_L(AnimationEventData& data) {
		auto giant = &data.giant;
		float scale = get_visual_scale(giant);
		DoCrawlingFunctions(giant, scale, 2.1f, Damage_Crawl_HandSlam_Strong, CrawlEvent::LeftHand, "RightHandRumble", 1.2f, Radius_Crawl_Slam_Strong, 1.0f, DamageSource::HandSlamLeft);
		DrainStamina(&data.giant, "StaminaDrain_CrawlStomp", Runtime::PERK.GTSPerkDestructionBasics, false, 1.4f);
		DrainStamina(&data.giant, "StaminaDrain_CrawlStompStrong", Runtime::PERK.GTSPerkDestructionBasics, false, 2.3f);
		Grab::DamageActorInHand(giant, Damage_Crawl_HandSlam_Strong * 0.6f);
	}

	void GTSCrawl_Slam_Cam_Off_R(AnimationEventData& data) {
		auto giant = &data.giant;
		EnableHandTracking(&data.giant, CrawlEvent::RightHand, false);
	}
	void GTSCrawl_Slam_Cam_Off_L(AnimationEventData& data) {
		auto giant = &data.giant;
		EnableHandTracking(&data.giant, CrawlEvent::LeftHand, false);
	}

	/////////////////////////////////////////////////////////Swipe Attacks//////////////////////////////////////////
	

	void TriggerHandCollision_Right(Actor* actor, float power, float crush, float pushpower) {
		std::string name = std::format("HandCollide_R_{}", actor->formID);
		auto gianthandle = actor->CreateRefHandle();

		std::vector<ObjectRefHandle> Objects = GetNearbyObjects(actor);

		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giant = gianthandle.get().get();
			auto Uarm = find_node(giant, "NPC R Forearm [RLar]");
			auto Arm = find_node(giant, "NPC R Hand [RHnd]");
			if (Uarm) {
				DoDamageAtPoint_Cooldown(giant, Radius_Sneak_HandSwipe, Damage_Crawl_HandSwipe * power, Uarm, NiPoint3(0,0,0), 10, 0.30f, crush, pushpower, DamageSource::HandSwipeRight);
				PushObjects(Objects, giant, Uarm, pushpower, Radius_Sneak_HandSwipe, false);
			}
			if (Arm) {
				DoDamageAtPoint_Cooldown(giant, Radius_Sneak_HandSwipe, Damage_Crawl_HandSwipe * power, Arm, NiPoint3(0,0,0), 10, 0.30f, crush, pushpower, DamageSource::HandSwipeRight);
				PushObjects(Objects, giant, Arm, pushpower, Radius_Sneak_HandSwipe, false);
			}
			return true;
		});
	}

	void TriggerHandCollision_Left(Actor* actor, float power, float crush, float pushpower) {
		std::string name = std::format("HandCollide_L_{}", actor->formID);
		auto gianthandle = actor->CreateRefHandle();

		std::vector<ObjectRefHandle> Objects = GetNearbyObjects(actor);

		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giant = gianthandle.get().get();
			auto Uarm = find_node(giant, "NPC L Forearm [LLar]");
			auto Arm = find_node(giant, "NPC L Hand [LHnd]");
			if (Uarm) {
				DoDamageAtPoint_Cooldown(giant, Radius_Sneak_HandSwipe, Damage_Crawl_HandSwipe * power, Uarm, NiPoint3(0,0,0), 10, 0.30f, crush, pushpower, DamageSource::HandSwipeLeft);
				PushObjects(Objects, giant, Uarm, pushpower, Radius_Sneak_HandSwipe, false);
			}
			if (Arm) {
				DoDamageAtPoint_Cooldown(giant, Radius_Sneak_HandSwipe, Damage_Crawl_HandSwipe * power, Arm, NiPoint3(0,0,0), 10, 0.30f, crush, pushpower, DamageSource::HandSwipeLeft);
				PushObjects(Objects, giant, Arm, pushpower, Radius_Sneak_HandSwipe, false);
			}
			return true;
		});
	}
	void DisableHandCollisions(Actor* actor) {
		std::string name = std::format("HandCollide_L_{}", actor->formID);
		std::string name2 = std::format("HandCollide_R_{}", actor->formID);
		TaskManager::Cancel(name);
		TaskManager::Cancel(name2);
	}

	void DisableHandTrackingTask(Actor* giant) { // Used to disable camera with some delay
		std::string name = std::format("CameraOff_{}", giant->formID);
		auto gianthandle = giant->CreateRefHandle();
		auto FrameA = Time::FramesElapsed();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giantref = gianthandle.get().get();
			auto FrameB = Time::FramesElapsed() - FrameA;
			if (FrameB <= 60.0f/GetAnimationSlowdown(giantref)) {
				return true;
			}

			EnableHandTracking(giantref, CrawlEvent::RightHand, false);
			EnableHandTracking(giantref, CrawlEvent::LeftHand, false);

			return false;
		});
	}

	void GTS_Crawl_Swipe_ArmSfx_Start(AnimationEventData& data) {
	}
	void GTS_Crawl_Swipe_ArmSfx_End(AnimationEventData& data) {
	}

	void GTS_Crawl_Swipe_On_R(AnimationEventData& data) {
		TriggerHandCollision_Right(&data.giant, 1.0f, 1.6f, Push_Crawl_HandSwipe);
		DrainStamina(&data.giant, "StaminaDrain_CrawlSwipe", Runtime::PERK.GTSPerkDestructionBasics, true, 4.0f);
	}
	void GTS_Crawl_Swipe_On_L(AnimationEventData& data) {
		TriggerHandCollision_Left(&data.giant, 1.0f, 1.6f, Push_Crawl_HandSwipe);
		DrainStamina(&data.giant, "StaminaDrain_CrawlSwipe", Runtime::PERK.GTSPerkDestructionBasics, true, 4.0f);
	}
	void GTS_Crawl_Swipe_Off_R(AnimationEventData& data) {
		DisableHandCollisions(&data.giant);
		DrainStamina(&data.giant, "StaminaDrain_CrawlSwipe", Runtime::PERK.GTSPerkDestructionBasics, false, 4.0f);
	}
	void GTS_Crawl_Swipe_Off_L(AnimationEventData& data) {
		DisableHandCollisions(&data.giant);
		DrainStamina(&data.giant, "StaminaDrain_CrawlSwipe", Runtime::PERK.GTSPerkDestructionBasics, false, 4.0f);
	}

	void GTS_Crawl_Swipe_Power_On_R(AnimationEventData& data) {
		TriggerHandCollision_Right(&data.giant, 2.0f, 1.3f, Push_Crawl_HandSwipe_Strong);
		DrainStamina(&data.giant, "StaminaDrain_CrawlSwipeStrong", Runtime::PERK.GTSPerkDestructionBasics, true, 10.0f);
	}
	void GTS_Crawl_Swipe_Power_On_L(AnimationEventData& data) {
		TriggerHandCollision_Left(&data.giant, 2.0f, 1.3f, Push_Crawl_HandSwipe_Strong);
		DrainStamina(&data.giant, "StaminaDrain_CrawlSwipeStrong", Runtime::PERK.GTSPerkDestructionBasics, true, 10.0f);
	}
	void GTS_Crawl_Swipe_Power_Off_R(AnimationEventData& data) {
		DisableHandCollisions(&data.giant);
		DrainStamina(&data.giant, "StaminaDrain_CrawlSwipeStrong", Runtime::PERK.GTSPerkDestructionBasics, false, 10.0f);
	}
	void GTS_Crawl_Swipe_Power_Off_L(AnimationEventData& data) {
		DisableHandCollisions(&data.giant);
		DrainStamina(&data.giant, "StaminaDrain_CrawlSwipeStrong", Runtime::PERK.GTSPerkDestructionBasics, false, 10.0f);
	}

	void LightSwipeLeftEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
			float WasteStamina = 25.0f * GetWasteMult(player);
			if (GetAV(player, ActorValue::kStamina) > WasteStamina) {
				Utils_UpdateHighHeelBlend(player, false);
				AnimationManager::StartAnim("SwipeLight_Left", player);
			} else {
				NotifyWithSound(player, "You're too tired for hand swipe");
			}
	}
	void LightSwipeRightEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
			float WasteStamina = 25.0f * GetWasteMult(player);
			if (GetAV(player, ActorValue::kStamina) > WasteStamina) {
				Utils_UpdateHighHeelBlend(player, false);
				AnimationManager::StartAnim("SwipeLight_Right", player);
			} else {
				NotifyWithSound(player, "You're too tired for hand swipe");
			}
	}

	void HeavySwipeLeftEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
			float WasteStamina = 70.0f * GetWasteMult(player);
			if (GetAV(player, ActorValue::kStamina) > WasteStamina) {
				Utils_UpdateHighHeelBlend(player, false);
				AnimationManager::StartAnim("SwipeHeavy_Left", player);
			} else {
				NotifyWithSound(player, "You're too tired for hand swipe");
			}
	}
	void HeavySwipeRightEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
			float WasteStamina = 70.0f * GetWasteMult(player);
			if (GetAV(player, ActorValue::kStamina) > WasteStamina) {
				Utils_UpdateHighHeelBlend(player, false);
				AnimationManager::StartAnim("SwipeHeavy_Right", player);
			} else {
				NotifyWithSound(player, "You're too tired for hand swipe");
			}
	}
}

namespace GTS
{
	void AnimationCrawling::RegisterEvents() {

		InputManager::RegisterInputEvent("LightSwipeLeft", LightSwipeLeftEvent, SwipeCondition);
		InputManager::RegisterInputEvent("LightSwipeRight", LightSwipeRightEvent, SwipeCondition);
		InputManager::RegisterInputEvent("HeavySwipeLeft", HeavySwipeLeftEvent, SwipeCondition);
		InputManager::RegisterInputEvent("HeavySwipeRight", HeavySwipeRightEvent, SwipeCondition);

		AnimationManager::RegisterEvent("GTS_Crawl_Knee_Trans_Impact", "Crawl", GTS_Crawl_Knee_Trans_Impact);
		AnimationManager::RegisterEvent("GTS_Crawl_Hand_Trans_Impact", "Crawl", GTS_Crawl_Hand_Trans_Impact);

		AnimationManager::RegisterEvent("GTSCrawl_KneeImpact_L", "Crawl", GTSCrawl_KneeImpact_L);
		AnimationManager::RegisterEvent("GTSCrawl_KneeImpact_R", "Crawl", GTSCrawl_KneeImpact_R);
		AnimationManager::RegisterEvent("GTSCrawl_HandImpact_L", "Crawl", GTSCrawl_HandImpact_L);
		AnimationManager::RegisterEvent("GTSCrawl_HandImpact_R", "Crawl", GTSCrawl_HandImpact_R);
		AnimationManager::RegisterEvent("GTSCrawl_Slam_Raise_Arm_R", "Crawl", GTSCrawl_Slam_Raise_Arm_R);
		AnimationManager::RegisterEvent("GTSCrawl_Slam_Raise_Arm_L", "Crawl", GTSCrawl_Slam_Raise_Arm_L);
		AnimationManager::RegisterEvent("GTSCrawl_SlamStrong_Raise_Arm_R", "Crawl", GTSCrawl_SlamStrong_Raise_Arm_R);
		AnimationManager::RegisterEvent("GTSCrawl_SlamStrong_Raise_Arm_L", "Crawl", GTSCrawl_SlamStrong_Raise_Arm_L);
		AnimationManager::RegisterEvent("GTSCrawl_Slam_Lower_Arm_R", "Crawl", GTSCrawl_Slam_Lower_Arm_R);
		AnimationManager::RegisterEvent("GTSCrawl_Slam_Lower_Arm_L", "Crawl", GTSCrawl_Slam_Lower_Arm_L);
		AnimationManager::RegisterEvent("GTSCrawl_SlamStrong_Lower_Arm_R", "Crawl", GTSCrawl_SlamStrong_Lower_Arm_R);
		AnimationManager::RegisterEvent("GTSCrawl_SlamStrong_Lower_Arm_L", "Crawl", GTSCrawl_SlamStrong_Lower_Arm_L);
		AnimationManager::RegisterEvent("GTSCrawl_Slam_Impact_R", "Crawl", GTSCrawl_Slam_Impact_R);
		AnimationManager::RegisterEvent("GTSCrawl_Slam_Impact_L", "Crawl", GTSCrawl_Slam_Impact_L);
		AnimationManager::RegisterEvent("GTSCrawl_SlamStrong_Impact_R", "Crawl", GTSCrawl_SlamStrong_Impact_R);
		AnimationManager::RegisterEvent("GTSCrawl_SlamStrong_Impact_L", "Crawl", GTSCrawl_SlamStrong_Impact_L);

		AnimationManager::RegisterEvent("GTSCrawl_Slam_Cam_Off_L", "Crawl", GTSCrawl_Slam_Cam_Off_L);
		AnimationManager::RegisterEvent("GTSCrawl_Slam_Cam_Off_R", "Crawl", GTSCrawl_Slam_Cam_Off_R);

		AnimationManager::RegisterEvent("GTS_Crawl_Swipe_On_R", "Crawl", GTS_Crawl_Swipe_On_R);
		AnimationManager::RegisterEvent("GTS_Crawl_Swipe_On_L", "Crawl", GTS_Crawl_Swipe_On_L);
		AnimationManager::RegisterEvent("GTS_Crawl_Swipe_Off_R", "Crawl", GTS_Crawl_Swipe_Off_R);
		AnimationManager::RegisterEvent("GTS_Crawl_Swipe_Off_L", "Crawl", GTS_Crawl_Swipe_Off_L);
		AnimationManager::RegisterEvent("GTS_Crawl_Swipe_Power_On_R", "Crawl", GTS_Crawl_Swipe_Power_On_R);
		AnimationManager::RegisterEvent("GTS_Crawl_Swipe_Power_On_L", "Crawl", GTS_Crawl_Swipe_Power_On_L);
		AnimationManager::RegisterEvent("GTS_Crawl_Swipe_Power_Off_R", "Crawl", GTS_Crawl_Swipe_Power_Off_R);
		AnimationManager::RegisterEvent("GTS_Crawl_Swipe_Power_Off_L", "Crawl", GTS_Crawl_Swipe_Power_Off_L);
		AnimationManager::RegisterEvent("GTS_Crawl_Swipe_ArmSfx_Start", "Crawl", GTS_Crawl_Swipe_ArmSfx_Start);
		AnimationManager::RegisterEvent("GTS_Crawl_Swipe_ArmSfx_End", "Crawl", GTS_Crawl_Swipe_ArmSfx_End);
	}

	void AnimationCrawling::RegisterTriggers() {
		AnimationManager::RegisterTrigger("SwipeLight_Left", "Crawl", "GTSBeh_SwipeLight_L");
		AnimationManager::RegisterTrigger("SwipeLight_Right", "Crawl", "GTSBeh_SwipeLight_R");
		AnimationManager::RegisterTrigger("SwipeHeavy_Right", "Crawl", "GTSBeh_SwipeHeavy_R");
		AnimationManager::RegisterTrigger("SwipeHeavy_Left", "Crawl", "GTSBeh_SwipeHeavy_L");
		AnimationManager::RegisterTrigger("CrawlON", "Crawl", "GTSBeh_Crawl_On");
		AnimationManager::RegisterTrigger("CrawlOFF", "Crawl", "GTSBeh_Crawl_Off");
	}
}