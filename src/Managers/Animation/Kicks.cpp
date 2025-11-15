#include "Managers/Animation/Kicks.hpp"
#include "Managers/Animation/AnimationManager.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Damage/LaunchObject.hpp"
#include "Managers/Input/InputManager.hpp"

#include "Utils/InputConditions.hpp"

using namespace GTS;

namespace {

	constexpr std::string_view RNode = "NPC R Foot [Rft ]";
	constexpr std::string_view LNode = "NPC L Foot [Lft ]";

	void PerformKick(std::string_view kick_type, float stamina_drain, bool strong) {
		auto player = PlayerCharacter::GetSingleton();
		std::string_view KickType;

		float WasteStamina = stamina_drain * GetWasteMult(player);
		if (GetAV(player, ActorValue::kStamina) > WasteStamina) {
			AnimationManager::StartAnim(kick_type, player);
		} else {
			strong ? KickType = "strong kick" : KickType = "light kick";
			NotifyWithSound(player, std::format("You're too tired for {}", KickType));
		}
	}

	void StartDamageAt(Actor* actor, float power, float crush, float pushpower, bool Right, std::string_view node, DamageSource Source) {
		std::string name = std::format("LegKick_{}", actor->formID);
		auto gianthandle = actor->CreateRefHandle();

		std::vector<ObjectRefHandle> Objects = GetNearbyObjects(actor);

		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giant = gianthandle.get().get();
			auto Leg = find_node(giant, node);
			if (Leg) {
				auto coords = GetFootCoordinates(actor, Right, false);
				if (!coords.empty()) {
					DoDamageAtPoint_Cooldown(giant, Radius_Kick, power, Leg, coords[1], 10, 0.30f, crush, pushpower, Source); // At Toe point
					PushObjects(Objects, giant, Leg, pushpower, Radius_Kick, true);
				}
			}
			return true;
		});
	}

	void StopAllDamageAndStamina(Actor* actor) {
		std::string name = std::format("LegKick_{}", actor->formID);
		DrainStamina(actor, "StaminaDrain_StrongKick", Runtime::PERK.GTSPerkDestructionBasics, false, 8.0f);
		DrainStamina(actor, "StaminaDrain_Kick", Runtime::PERK.GTSPerkDestructionBasics, false, 4.0f);
		TaskManager::Cancel(name);
	}

	void GTS_Kick_Camera_On_R(AnimationEventData& data) {
		ManageCamera(&data.giant, true, CameraTracking::R_Foot);
	}
	void GTS_Kick_Camera_On_L(AnimationEventData& data) {
		ManageCamera(&data.giant, true, CameraTracking::L_Foot);
	}
	void GTS_Kick_Camera_Off_R(AnimationEventData& data) {
		ManageCamera(&data.giant, false, CameraTracking::R_Foot);
	}
	void GTS_Kick_Camera_Off_L(AnimationEventData& data) {
		ManageCamera(&data.giant, false, CameraTracking::L_Foot);
	}

	void GTS_Kick_SwingLeg_L(AnimationEventData& data) {
	}
	void GTS_Kick_SwingLeg_R(AnimationEventData& data) {
	}

	void GTS_Kick_HitBox_On_R(AnimationEventData& data) {
		StartDamageAt(&data.giant, Damage_Kick, 1.8f, Push_Kick_Normal, true, "NPC R Toe0 [RToe]", DamageSource::KickedRight);
		DrainStamina(&data.giant, "StaminaDrain_StrongKick", Runtime::PERK.GTSPerkDestructionBasics, true, 4.0f);
	}
	void GTS_Kick_HitBox_On_L(AnimationEventData& data) {
		StartDamageAt(&data.giant, Damage_Kick, 1.8f, Push_Kick_Normal, false, "NPC L Toe0 [LToe]", DamageSource::KickedLeft);
		DrainStamina(&data.giant, "StaminaDrain_StrongKick", Runtime::PERK.GTSPerkDestructionBasics, true, 4.0f);
	}
	void GTS_Kick_HitBox_Off_R(AnimationEventData& data) {
		StopAllDamageAndStamina(&data.giant);
	}
	void GTS_Kick_HitBox_Off_L(AnimationEventData& data) {
		StopAllDamageAndStamina(&data.giant);
	}

	void GTS_Kick_HitBox_Power_On_R(AnimationEventData& data) {
		StartDamageAt(&data.giant, Damage_Kick_Strong, 1.8f, Push_Kick_Strong, true, "NPC R Toe0 [RToe]", DamageSource::KickedRight);
		DrainStamina(&data.giant, "StaminaDrain_StrongKick", Runtime::PERK.GTSPerkDestructionBasics, true, 8.0f);
	}
	void GTS_Kick_HitBox_Power_On_L(AnimationEventData& data) {
		StartDamageAt(&data.giant, Damage_Kick_Strong, 1.8f, Push_Kick_Strong, false, "NPC L Toe0 [LToe]", DamageSource::KickedLeft);
		DrainStamina(&data.giant, "StaminaDrain_StrongKick", Runtime::PERK.GTSPerkDestructionBasics, true, 8.0f);
	}
	void GTS_Kick_HitBox_Power_Off_R(AnimationEventData& data) {
		StopAllDamageAndStamina(&data.giant);
	}
	void GTS_Kick_HitBox_Power_Off_L(AnimationEventData& data) {
		StopAllDamageAndStamina(&data.giant);
	}

	// ======================================================================================
	//  Animation Triggers
	// ======================================================================================
	void LightKickLeftEvent(const ManagedInputEvent& data) {
		PerformKick("SwipeLight_Left", 35.0f, false);
	}
	void LightKickRightEvent(const ManagedInputEvent& data) {
		PerformKick("SwipeLight_Right", 35.0f, false);
	}

	void HeavyKickLeftEvent(const ManagedInputEvent& data) {
		PerformKick("SwipeHeavy_Left", 110.0f, true);
	}
	void HeavyKickRightEvent(const ManagedInputEvent& data) {
		PerformKick("SwipeHeavy_Right", 110.0f, true);
	}

	void HeavyKickRightLowEvent(const ManagedInputEvent& data) {
		PerformKick("StrongKick_Low_Right", 110.0f, true);
	}
	void HeavyKickLeftLowEvent(const ManagedInputEvent& data) {
		PerformKick("StrongKick_Low_Left", 110.0f, true);
	}
}

namespace GTS
{
	void AnimationKicks::RegisterEvents() {
		InputManager::RegisterInputEvent("LightKickLeft", LightKickLeftEvent, KickCondition);
		InputManager::RegisterInputEvent("LightKickRight", LightKickRightEvent, KickCondition);
		InputManager::RegisterInputEvent("HeavyKickLeft", HeavyKickLeftEvent, KickCondition);
		InputManager::RegisterInputEvent("HeavyKickRight", HeavyKickRightEvent, KickCondition);
		InputManager::RegisterInputEvent("HeavyKickRight_Low", HeavyKickRightLowEvent, KickCondition);
		InputManager::RegisterInputEvent("HeavyKickLeft_Low", HeavyKickLeftLowEvent, KickCondition);
		

		AnimationManager::RegisterEvent("GTS_Kick_Camera_On_R", "Kicks", GTS_Kick_Camera_On_R);
		AnimationManager::RegisterEvent("GTS_Kick_Camera_On_L", "Kicks", GTS_Kick_Camera_On_L);
		AnimationManager::RegisterEvent("GTS_Kick_Camera_Off_R", "Kicks", GTS_Kick_Camera_On_R);
		AnimationManager::RegisterEvent("GTS_Kick_Camera_Off_L", "Kicks", GTS_Kick_Camera_On_L);

		AnimationManager::RegisterEvent("GTS_Kick_SwingLeg_R", "Kicks", GTS_Kick_SwingLeg_R);
		AnimationManager::RegisterEvent("GTS_Kick_SwingLeg_L", "Kicks", GTS_Kick_SwingLeg_L);

		AnimationManager::RegisterEvent("GTS_Kick_HitBox_On_R", "Kicks", GTS_Kick_HitBox_On_R);
		AnimationManager::RegisterEvent("GTS_Kick_HitBox_Off_R", "Kicks", GTS_Kick_HitBox_Off_R);
		AnimationManager::RegisterEvent("GTS_Kick_HitBox_On_L", "Kicks", GTS_Kick_HitBox_On_L);
		AnimationManager::RegisterEvent("GTS_Kick_HitBox_Off_L", "Kicks", GTS_Kick_HitBox_Off_L);

		AnimationManager::RegisterEvent("GTS_Kick_HitBox_Power_On_R", "Kicks", GTS_Kick_HitBox_Power_On_R);
		AnimationManager::RegisterEvent("GTS_Kick_HitBox_Power_Off_R", "Kicks", GTS_Kick_HitBox_Power_Off_R);
		AnimationManager::RegisterEvent("GTS_Kick_HitBox_Power_On_L", "Kicks", GTS_Kick_HitBox_Power_On_L);
		AnimationManager::RegisterEvent("GTS_Kick_HitBox_Power_Off_L", "Kicks", GTS_Kick_HitBox_Power_Off_L);
	}

	void AnimationKicks::RegisterTriggers() {
		AnimationManager::RegisterTrigger("StrongKick_Low_Left", "Kicks", "GTSBEH_HeavyKickLow_L");
		AnimationManager::RegisterTrigger("StrongKick_Low_Right", "Kicks", "GTSBEH_HeavyKickLow_R");
	}
}