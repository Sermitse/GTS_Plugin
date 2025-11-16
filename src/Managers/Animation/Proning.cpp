#include "Managers/Animation/Proning.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CrawlUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Managers/Rumble.hpp"

#include "Magic/Effects/Common.hpp"

#include "Utils/Actions/InputConditions.hpp"

using namespace GTS;

namespace {

    const std::vector<std::string_view> BODY_NODES = {
		"NPC R Thigh [RThg]",
		"NPC L Thigh [LThg]",
		"NPC R Butt",
		"NPC L Butt",
		"NPC Spine [Spn0]",
		"NPC Spine1 [Spn1]",
		"NPC Spine2 [Spn2]",
	};

    void StartBodyDamage_DOT(Actor* giant) {
		float damage = 2.0f * TimeScale();
		auto gianthandle = giant->CreateRefHandle();
		std::string name = std::format("BodyDOT_{}", giant->formID);
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giantref = gianthandle.get().get();

			auto BreastL = find_node(giantref, "NPC L Breast");
			auto BreastR = find_node(giantref, "NPC R Breast");
			auto BreastL03 = find_node(giantref, "L Breast03");
			auto BreastR03 = find_node(giantref, "R Breast03");

			if (!IsProning(giantref)) {
				return false;
			}

			for (auto Nodes: BODY_NODES) {
				auto Node = find_node(giantref, Nodes);
				if (Node) {
					DoDamageAtPoint(giant, Radius_Proning_BodyDOT, Damage_BreastCrush_BodyDOT * damage, Node, 400, 0.10f, 1.33f, DamageSource::BodyCrush);
				}
			}

			ApplyThighDamage(giant, true, false, Radius_ThighCrush_Idle, Damage_BreastCrush_BodyDOT * damage * 0.6f, 0.10f, 1.6f, 200, DamageSource::ThighCrushed);
			ApplyThighDamage(giant, false, false, Radius_ThighCrush_Idle, Damage_BreastCrush_BodyDOT * damage * 0.6f, 0.10f, 1.6f, 200, DamageSource::ThighCrushed);

			if (BreastL03 && BreastR03) {
				DoDamageAtPoint(giant, Radius_BreastCrush_BreastDOT, Damage_BreastCrush_BreastDOT * damage, BreastL03, 400, 0.10f, 1.33f, DamageSource::BreastImpact);
				DoDamageAtPoint(giant, Radius_BreastCrush_BreastDOT, Damage_BreastCrush_BreastDOT * damage, BreastR03, 400, 0.10f, 1.33f, DamageSource::BreastImpact);
				return true;
			} else if (BreastL && BreastR) {
				DoDamageAtPoint(giant, Radius_BreastCrush_BreastDOT, Damage_BreastCrush_BreastDOT * damage, BreastL, 400, 0.10f, 1.33f, DamageSource::BreastImpact);
				DoDamageAtPoint(giant, Radius_BreastCrush_BreastDOT, Damage_BreastCrush_BreastDOT * damage, BreastR, 400, 0.10f, 1.33f, DamageSource::BreastImpact);
				return true;
			}
			return false;
		});
	}

	void StartBodyDamage_Slide(Actor* giant) {
		float damage = 18.0f * TimeScale();
		auto gianthandle = giant->CreateRefHandle();
		std::string name = std::format("BodyDOT_Slide_{}", giant->formID);

		auto BreastL = find_node(giant, "NPC L Breast");
		auto BreastR = find_node(giant, "NPC R Breast");
		auto BreastL03 = find_node(giant, "L Breast03");
		auto BreastR03 = find_node(giant, "R Breast03");

		for (auto Nodes: BODY_NODES) {
			auto Node = find_node(giant, Nodes);
			if (Node) {
				std::string rumbleName = std::format("Node: {}", Nodes);
				Rumbling::Once(rumbleName, giant, 0.10f, 0.02f, Nodes, 0.0f);
			}
		}

		if (BreastL03 && BreastR03) {
			Rumbling::Once("BreastDot_L", giant, 0.10f, 0.025f, "L Breast03", 0.0f);
			Rumbling::Once("BreastDot_R", giant, 0.10f, 0.025f, "R Breast03", 0.0f);
		} else if (BreastL && BreastR) {
			Rumbling::Once("BreastDot_L", giant, 0.10f, 0.025f, "NPC L Breast", 0.0f);
			Rumbling::Once("BreastDot_R", giant, 0.10f, 0.025f, "NPC R Breast", 0.0f);
		}
		
		

		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giantref = gianthandle.get().get();

			auto BreastL = find_node(giantref, "NPC L Breast");
			auto BreastR = find_node(giantref, "NPC R Breast");
			auto BreastL03 = find_node(giantref, "L Breast03");
			auto BreastR03 = find_node(giantref, "R Breast03");

			if (!IsProning(giantref)) {
				return false;
			}

			for (auto Nodes: BODY_NODES) {
				auto Node = find_node(giantref, Nodes);
				if (Node) {
					DoDamageAtPoint(giant, Radius_BreastCrush_BodyDOT, Damage_BreastCrush_BodyDOT * damage, Node, 200, 0.10f, 1.0f, DamageSource::BodyCrush);
				}
			}

			ApplyThighDamage(giant, true, false, Radius_ThighCrush_Idle, Damage_BreastCrush_BodyDOT * damage * 0.6f, 0.10f, 1.0f, 200, DamageSource::ThighCrushed);
			ApplyThighDamage(giant, false, false, Radius_ThighCrush_Idle, Damage_BreastCrush_BodyDOT * damage * 0.6f, 0.10f, 1.0f, 200, DamageSource::ThighCrushed);

			if (BreastL03 && BreastR03) {
				DoDamageAtPoint(giant, Radius_BreastCrush_BreastDOT, Damage_BreastCrush_BreastDOT * damage, BreastL03, 200, 0.10f, 1.0f, DamageSource::BreastImpact);
				DoDamageAtPoint(giant, Radius_BreastCrush_BreastDOT, Damage_BreastCrush_BreastDOT * damage, BreastR03, 200, 0.10f, 1.0f, DamageSource::BreastImpact);
				return true;
			} else if (BreastL && BreastR) {
				DoDamageAtPoint(giant, Radius_BreastCrush_BreastDOT, Damage_BreastCrush_BreastDOT * damage, BreastL, 200, 0.10f, 1.0f, DamageSource::BreastImpact);
				DoDamageAtPoint(giant, Radius_BreastCrush_BreastDOT, Damage_BreastCrush_BreastDOT * damage, BreastR, 200, 0.10f, 1.0f, DamageSource::BreastImpact);
				return true;
			}
			return false;
		});
	}

    void StopBodyDamage_Slide(Actor* giant) {
		std::string name = std::format("BodyDOT_Slide_{}", giant->formID);
		TaskManager::Cancel(name);
	}

    //////////////////////////////////////////////////////
    ///// E V E N T S
    //////////////////////////////////////////////////////

    void GTS_DiveSlide_ON(AnimationEventData& data) {
		auto giant = &data.giant;
		StartBodyDamage_Slide(giant);
	}
	void GTS_DiveSlide_OFF(AnimationEventData& data) {
		auto giant = &data.giant;
		StopBodyDamage_Slide(giant);
	}
	void GTS_BodyDamage_ON(AnimationEventData& data) {
		auto giant = &data.giant;
		SetProneState(giant, true);
		StartBodyDamage_DOT(giant);
	}
	void GTS_BodyDamage_Off(AnimationEventData& data) {
		auto giant = &data.giant;
		SetProneState(giant, false);
	}

	void GTS_Prone_EnterSneak(AnimationEventData& data) {
		Actor* giant = &data.giant;
		SetSneaking(giant, true, 1);
		giant->SetGraphVariableInt("iIsInSneak", 1);
	}

	void SBOProneOnEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		AnimationManager::StartAnim("SBO_ProneOn", player);
	}

	void SBOProneOffEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();

		bool prone = false;
		player->GetGraphVariableBool("GTS_IsProne", prone);
		
		if (prone) {
			AnimationManager::StartAnim("SBO_ProneOff", player);
		}
	}

	void SBODiveEvent_Standing(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		if (!player->IsSneaking()) {
			AnimationManager::StartAnim("SBO_Dive", player);
		}
	}

	void SBODiveEvent_Sneak(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		if (player->IsSneaking()) {
			AnimationManager::StartAnim("SBO_Dive", player);
		}
	}


}

namespace GTS
{
	void AnimationProning::RegisterEvents() {
		InputManager::RegisterInputEvent("SBO_ToggleProne", SBOProneOnEvent, AlwaysBlock);
		InputManager::RegisterInputEvent("SBO_DisableProne", SBOProneOffEvent, AlwaysBlock);
		InputManager::RegisterInputEvent("SBO_ToggleDive_Standing", SBODiveEvent_Standing, AlwaysBlock);
		InputManager::RegisterInputEvent("SBO_ToggleDive_Sneak", SBODiveEvent_Sneak, AlwaysBlock);

		AnimationManager::RegisterEvent("GTS_DiveSlide_ON", "Proning", GTS_DiveSlide_ON);
		AnimationManager::RegisterEvent("GTS_DiveSlide_OFF", "Proning", GTS_DiveSlide_OFF);
		AnimationManager::RegisterEvent("GTS_BodyDamage_ON", "Proning", GTS_BodyDamage_ON);
		AnimationManager::RegisterEvent("GTS_BodyDamage_Off", "Proning", GTS_BodyDamage_Off);
		AnimationManager::RegisterEvent("GTS_Prone_EnterSneak", "Pronong", GTS_Prone_EnterSneak);
	}

	void  AnimationProning::RegisterTriggers() {
		AnimationManager::RegisterTrigger("SBO_ProneOn", "Proning", "GTSBeh_ProneStart");
		AnimationManager::RegisterTrigger("SBO_ProneOff", "Proning", "GTSBeh_ProneStop");
		AnimationManager::RegisterTrigger("SBO_Dive", "Proning", "GTSBeh_ProneStart_Dive");
	}
}