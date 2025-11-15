#include "Managers/Animation/FootGrind.hpp"
#include "Managers/Animation/AnimationManager.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"

#include "Managers/Audio/Footstep.hpp"
#include "Managers/Audio/Stomps.hpp"
#include "Managers/Explosion.hpp"
#include "Managers/Rumble.hpp"

#include "Magic/Effects/Common.hpp"

using namespace GTS;

namespace {
	constexpr std::string_view RNode = "NPC R Foot [Rft ]";
	constexpr std::string_view LNode = "NPC L Foot [Lft ]";

	void CancelGrindTasks(Actor* giant) { // Disable all grind stuff
		std::string task_name_1 = std::format("FootGrind_{}_{}", giant->formID, "Left_Light");
		std::string task_name_2 = std::format("FootGrind_{}_{}", giant->formID, "Right_Light");
		std::string dot_name = std::format("FootGrindDOT_{}", giant->formID);
		std::string rot_name = std::format("FootGrindRot_{}", giant->formID);

		if (IsFootGrinding(giant)) {
			giant->SetGraphVariableBool("GTS_IsFootGrinding", false); // stop foot grind manually
		}

		TaskManager::Cancel(task_name_1);
		TaskManager::Cancel(task_name_2);
		TaskManager::Cancel(dot_name);
		TaskManager::Cancel(rot_name);
	}

	void ResetGrindData(AnimationEventData& data) { // Restores data to natural values
		data.stage = 0;
		data.canEditAnimSpeed = false;
		data.animSpeed = 1.0f;
	}

	void FixAttachedTiny(AnimationEventData& data) {
		// The purpose of this function is to fix Sonderbain's foot grind anims (I don't want to ask Sonder to redo events because of this issue)
		// The issue: Sonder's anim has a bit wrong timing of _Exit event so Tiny stays attached to the foot when it's not needed (for ~ 1 sec)
		// This function fixes the issue
		if (!IsUsingAlternativeStomp(&data.giant) && !IsUnderGrinding(&data.giant)) {
			if (data.stage >= 7) {
				CancelGrindTasks(&data.giant);
				data.stage = 0; // reset stage
			}
		}
	}

	void ApplyDustRing(Actor* giant, FootEvent kind, std::string_view node, float mult) {
		auto& explosion = ExplosionManager::GetSingleton();
		Impact impact_data = Impact {
			.actor = giant,
			.kind = kind,
			.scale = get_visual_scale(giant),
			.modifier = mult,
			.nodes = find_node(giant, node),
		};
		explosion.OnImpact(impact_data); // Play explosion
	}

	void ApplyDamageOverTime(Actor* giant, std::string_view node, FootEvent Event, std::string_view task_name) {
		auto gianthandle = giant->CreateRefHandle();
		std::string r_name = std::format("FootGrindDOT_{}", giant->formID);
		std::string name = std::format("FootGrind_{}_{}", giant->formID, task_name);
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			} 
			auto giantref = gianthandle.get().get();
			if (!IsFootGrinding(giantref)) {
				return false; 
			}
			Laugh_Chance(giantref, 2.2f, "FootGrind");

			Rumbling::Once(r_name, giantref, Rumble_FootGrind_DOT, 0.025f, RNode, 0.0f);
			float speed = AnimationManager::GetBonusAnimationSpeed(giant) * TimeScale();
			DoDamageEffect(giantref, Damage_Foot_Grind_DOT * speed, Radius_Foot_Grind_DOT, 10000, 0.025f, Event, 2.5f, DamageSource::FootGrindedRight);
			return true;
		});
	}

	void ApplyRotateDamage(Actor* giant, std::string_view node, FootEvent kind, DamageSource source) {
		Laugh_Chance(giant, 2.2f, "FootGrind");
		float speed = AnimationManager::GetBonusAnimationSpeed(giant);
		float damage_mult = 1.0f;

		if (IsUsingAlternativeStomp(giant)) {
			damage_mult = 0.6f; // Since there's more total rotate events (15 vs 7)
		}

		std::string r_name = std::format("FootGrindRot_{}", giant->formID);

		float DOT = Damage_Foot_Grind_Rotate;
		float ring_radius = 0.9f;

		if (giant->IsSneaking()) {
			ring_radius *= 0.85f;
			DOT *= 2.25f; // Has less rotation events so we buff the damage a bit
		}

		Rumbling::Once(r_name, giant, Rumble_FootGrind_Rotate * speed, 0.025f, node, 0.0f);
		DoDamageEffect(giant, DOT, Radius_Foot_Grind_DOT, 10, 0.15f, kind, 1.6f, source);

		ApplyDustRing(giant, kind, node, ring_radius);
	}

	void Footgrind_DoImpact(Actor* giant, bool right, FootEvent Event, DamageSource Source, std::string_view Node, std::string_view rumble) {
		float perk = GetPerkBonus_Basics(giant);
		ApplyDustRing(giant, Event, Node, 1.05f);
		StompManager::PlayNewOrOldStomps(giant, 1.0f, Event, Node, false);

		DoDamageEffect(giant, Damage_Foot_Grind_Impact, Radius_Foot_Grind_Impact, 20, 0.15f, Event, 1.0f, Source);
		LaunchTask(giant, 0.75f * perk, 1.35f * perk, Event);

		DamageAV(giant, ActorValue::kStamina, 30.0f * GetWasteMult(giant));

		float shake_power = Rumble_FootGrind_Impact * GetHighHeelsBonusDamage(giant, true);

		if (HasSMT(giant)) {
			shake_power *= 1.5f;
		}
		
		Rumbling::Once(rumble, giant, shake_power, 0.05f, Node, 0.0f);
		FootStepManager::PlayVanillaFootstepSounds(giant, right);
	}

	//////////////////////////////////////////////////////////////////
	/// Events
	//////////////////////////////////////////////////////////////////

	void GTSstomp_FootGrindL_Enter(AnimationEventData& data) {
		data.stage = 1;
		data.canEditAnimSpeed = true;
		data.animSpeed = 1.0f;
		DrainStamina(&data.giant, "StaminaDrain_FootGrind", Runtime::PERK.GTSPerkDestructionBasics, true, 0.25f);
		ApplyDamageOverTime(&data.giant, LNode, FootEvent::Left, "Left_Light");
	}

	void GTSstomp_FootGrindR_Enter(AnimationEventData& data) {
		data.stage = 1;
		data.canEditAnimSpeed = true;
		data.animSpeed = 1.0f;
		DrainStamina(&data.giant, "StaminaDrain_FootGrind", Runtime::PERK.GTSPerkDestructionBasics, true, 0.25f);
		ApplyDamageOverTime(&data.giant, RNode, FootEvent::Right, "Right_Light");
	}

	void GTSstomp_FootGrindL_MV_S(AnimationEventData& data) { // Feet starts to move: Left
		ApplyRotateDamage(&data.giant, LNode, FootEvent::Left, DamageSource::FootGrindedLeft);
		data.stage += 1; // Rotation is done 6 times in total
	}

	void GTSstomp_FootGrindR_MV_S(AnimationEventData& data) { // Feet start to move: Right
		ApplyRotateDamage(&data.giant, RNode, FootEvent::Right, DamageSource::FootGrindedRight);
		data.stage += 1; // Rotation is done 6 times in total
	}

	void GTSstomp_FootGrindL_MV_E(AnimationEventData& data) { // When movement ends: Left
		ApplyDustRing(&data.giant, FootEvent::Left, LNode, 0.9f);
		FixAttachedTiny(data); // Fix for SonderBain's anim ONLY
	}

	void GTSstomp_FootGrindR_MV_E(AnimationEventData& data) { // When movement ends: Right
		ApplyDustRing(&data.giant, FootEvent::Right, RNode, 0.9f);
		FixAttachedTiny(data); // Fix for SonderBain's anim ONLY
	}

	void GTSstomp_FootGrindR_Impact(AnimationEventData& data) { // When foot hits the ground after lifting the leg up. R Foot
		Footgrind_DoImpact(&data.giant, true, FootEvent::Right, DamageSource::FootGrindedRight_Impact, RNode, "GrindStompR");
	}

	void GTSstomp_FootGrindL_Impact(AnimationEventData& data) { // When foot hits the ground after lifting the leg up. L Foot
		Footgrind_DoImpact(&data.giant, false, FootEvent::Left, DamageSource::FootGrindedLeft_Impact, LNode, "GrindStompL");
	}

	void GTSstomp_FootGrindR_Exit(AnimationEventData& data) { // Called when we want to deattach tiny from the foot and end grind in general
		DrainStamina(&data.giant, "StaminaDrain_FootGrind", Runtime::PERK.GTSPerkDestructionBasics, false, 0.25f);
		CancelGrindTasks(&data.giant);
		ResetGrindData(data);
	}

	void GTSstomp_FootGrindL_Exit(AnimationEventData& data) { // Called when we want to deattach tiny from the foot and end grind in general
		DrainStamina(&data.giant, "StaminaDrain_FootGrind", Runtime::PERK.GTSPerkDestructionBasics, false, 0.25f);
		CancelGrindTasks(&data.giant);
		ResetGrindData(data);
	}
}

namespace GTS
{
	void AnimationFootGrind::RegisterEvents() {
		AnimationManager::RegisterEvent("GTSstomp_FootGrindL_Enter", "Stomp", GTSstomp_FootGrindL_Enter);
		AnimationManager::RegisterEvent("GTSstomp_FootGrindR_Enter", "Stomp", GTSstomp_FootGrindR_Enter);
		AnimationManager::RegisterEvent("GTSstomp_FootGrindL_MV_S", "Stomp", GTSstomp_FootGrindL_MV_S);
		AnimationManager::RegisterEvent("GTSstomp_FootGrindR_MV_S", "Stomp", GTSstomp_FootGrindR_MV_S);
		AnimationManager::RegisterEvent("GTSstomp_FootGrindL_MV_E", "Stomp", GTSstomp_FootGrindL_MV_E);
		AnimationManager::RegisterEvent("GTSstomp_FootGrindR_MV_E", "Stomp", GTSstomp_FootGrindR_MV_E);
		AnimationManager::RegisterEvent("GTSstomp_FootGrindR_Impact", "Stomp", GTSstomp_FootGrindR_Impact);
		AnimationManager::RegisterEvent("GTSstomp_FootGrindL_Impact", "Stomp", GTSstomp_FootGrindL_Impact);
		AnimationManager::RegisterEvent("GTSstomp_FootGrindR_Exit", "Stomp", GTSstomp_FootGrindR_Exit);
		AnimationManager::RegisterEvent("GTSstomp_FootGrindL_Exit", "Stomp", GTSstomp_FootGrindL_Exit);
	}

	void AnimationFootGrind::RegisterTriggers() {
		AnimationManager::RegisterTrigger("GrindRight", "Stomp", "GTSBEH_StartFootGrindR");
		AnimationManager::RegisterTrigger("GrindLeft", "Stomp", "GTSBEH_StartFootGrindL");
	}
}