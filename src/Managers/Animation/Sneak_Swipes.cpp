#include "Managers/Animation/Sneak_Swipes.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"

#include "Managers/Damage/LaunchObject.hpp"

using namespace GTS;

namespace { 
	void TriggerHandCollision_Right(Actor* actor, float power, float crush, float pushpower) {
		std::string name = std::format("SwipeCollide_R_{}", actor->formID);
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
				DoDamageAtPoint_Cooldown(giant, Radius_Sneak_HandSwipe, power, Uarm, NiPoint3(0,0,0), 10, 0.30f, crush, pushpower, DamageSource::HandSwipeRight);
				PushObjects(Objects, actor, Uarm, pushpower, Radius_Sneak_HandSwipe, false);
			}
			if (Arm) {
				DoDamageAtPoint_Cooldown(giant, Radius_Sneak_HandSwipe, power, Arm, NiPoint3(0,0,0), 10, 0.30f, crush, pushpower, DamageSource::HandSwipeRight);
				PushObjects(Objects, actor, Arm, pushpower, Radius_Sneak_HandSwipe, false);
			}

			Utils_UpdateHighHeelBlend(giant, false);

			return true;
		});
	}

	void TriggerHandCollision_Left(Actor* actor, float power, float crush, float pushpower) {
		std::string name = std::format("SwipeCollide_L_{}", actor->formID);
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
				DoDamageAtPoint_Cooldown(giant, Radius_Sneak_HandSwipe, power, Uarm, NiPoint3(0,0,0), 10, 0.30f, crush, pushpower, DamageSource::HandSwipeLeft);
				PushObjects(Objects, actor, Uarm, pushpower, Radius_Sneak_HandSwipe, false);
			}
			if (Arm) {
				DoDamageAtPoint_Cooldown(giant, Radius_Sneak_HandSwipe, power, Arm, NiPoint3(0,0,0), 10, 0.30f, crush, pushpower, DamageSource::HandSwipeLeft);
				PushObjects(Objects, actor, Arm, pushpower, Radius_Sneak_HandSwipe, false);
			}

			Utils_UpdateHighHeelBlend(giant, false);

			return true;
		});
	}
	void DisableHandCollisions(Actor* actor) {
		std::string name = std::format("SwipeCollide_L_{}", actor->formID);
		std::string name2 = std::format("SwipeCollide_R_{}", actor->formID);
		TaskManager::Cancel(name);
		TaskManager::Cancel(name2);
	}

	void GTS_Sneak_Swipe_ArmSfx_Start(AnimationEventData& data) {
	}
	void GTS_Sneak_Swipe_ArmSfx_End(AnimationEventData& data) {
	}

	////////////////light

	void GTS_Sneak_Swipe_On_R(AnimationEventData& data) {
		TriggerHandCollision_Right(&data.giant, Damage_Sneak_HandSwipe, 1.8f, Push_Sneak_HandSwipe);
		DrainStamina(&data.giant, "StaminaDrain_CrawlSwipe", Runtime::PERK.GTSPerkDestructionBasics, true, 4.0f);
	}
	void GTS_Sneak_Swipe_On_L(AnimationEventData& data) {
		TriggerHandCollision_Left(&data.giant, Damage_Sneak_HandSwipe, 1.8f, Push_Sneak_HandSwipe);
		DrainStamina(&data.giant, "StaminaDrain_CrawlSwipe", Runtime::PERK.GTSPerkDestructionBasics, true, 4.0f);
	}
	void GTS_Sneak_Swipe_Off_R(AnimationEventData& data) {
		DrainStamina(&data.giant, "StaminaDrain_CrawlSwipe", Runtime::PERK.GTSPerkDestructionBasics, false, 4.0f);
		DisableHandCollisions(&data.giant);
	}
	void GTS_Sneak_Swipe_Off_L(AnimationEventData& data) {
		DrainStamina(&data.giant, "StaminaDrain_CrawlSwipe", Runtime::PERK.GTSPerkDestructionBasics, false, 4.0f);
		DisableHandCollisions(&data.giant);
	}

	///////////////strong

	void GTS_Sneak_Swipe_Power_On_R(AnimationEventData& data) {
		DrainStamina(&data.giant, "StaminaDrain_CrawlSwipeStrong", Runtime::PERK.GTSPerkDestructionBasics, true, 10.0f);
		TriggerHandCollision_Right(&data.giant, Damage_Sneak_HandSwipe_Strong, 1.4f, Push_Sneak_HandSwipe_Strong);
	}
	void GTS_Sneak_Swipe_Power_On_L(AnimationEventData& data) {
		DrainStamina(&data.giant, "StaminaDrain_CrawlSwipeStrong", Runtime::PERK.GTSPerkDestructionBasics, true, 10.0f);
		TriggerHandCollision_Left(&data.giant, Damage_Sneak_HandSwipe_Strong, 1.4f, Push_Sneak_HandSwipe_Strong);
	}
	void GTS_Sneak_Swipe_Power_Off_R(AnimationEventData& data) {
		DrainStamina(&data.giant, "StaminaDrain_CrawlSwipeStrong", Runtime::PERK.GTSPerkDestructionBasics, false, 10.0f);
		DisableHandCollisions(&data.giant);
	}
	void GTS_Sneak_Swipe_Power_Off_L(AnimationEventData& data) {
		DrainStamina(&data.giant, "StaminaDrain_CrawlSwipeStrong", Runtime::PERK.GTSPerkDestructionBasics, false, 10.0f);
		DisableHandCollisions(&data.giant);
	}
}

namespace GTS {
    
    void Animation_SneakSwipes::RegisterEvents() {
		AnimationManager::RegisterEvent("GTS_Sneak_Swipe_On_R", "Sneak", GTS_Sneak_Swipe_On_R);
		AnimationManager::RegisterEvent("GTS_Sneak_Swipe_On_L", "Sneak", GTS_Sneak_Swipe_On_L);
		AnimationManager::RegisterEvent("GTS_Sneak_Swipe_Off_R", "Sneak", GTS_Sneak_Swipe_Off_R);
		AnimationManager::RegisterEvent("GTS_Sneak_Swipe_Off_L", "Sneak", GTS_Sneak_Swipe_Off_L);


		AnimationManager::RegisterEvent("GTS_Sneak_Swipe_Power_On_R", "Sneak", GTS_Sneak_Swipe_Power_On_R);
		AnimationManager::RegisterEvent("GTS_Sneak_Swipe_Power_On_L", "Sneak", GTS_Sneak_Swipe_Power_On_L);
		AnimationManager::RegisterEvent("GTS_Sneak_Swipe_Power_Off_R", "Sneak", GTS_Sneak_Swipe_Power_Off_R);
		AnimationManager::RegisterEvent("GTS_Sneak_Swipe_Power_Off_L", "Sneak", GTS_Sneak_Swipe_Power_Off_L);
    }
}
