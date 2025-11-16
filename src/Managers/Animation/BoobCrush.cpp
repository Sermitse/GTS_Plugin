#include "Managers/Animation/BoobCrush.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CrawlUtils.hpp"
#include "Managers/Rumble.hpp"
#include "Magic/Effects/Common.hpp"
#include "Utils/Actions/ButtCrushUtils.hpp"

#include "Managers/Audio/MoansLaughs.hpp"

using namespace GTS;

/*
   GTS_BoobCrush_Smile_On
   GTS_BoobCrush_Smile_Off
   GTS_BoobCrush_TrackBody			(Enables camera tracking)
   GTS_BoobCrush_UnTrackBody		(Disables it)
   GTS_BoobCrush_BreastImpact		(Damage everything under breasts, on impact)
   GTS_BoobCrush_DOT_Start			(When we want to deal damage over time when we do long idle with swaying legs)
   GTS_BoobCrush_DOT_End
   GTS_BoobCrush_Grow_Start
   GTS_BoobCrush_Grow_Stop
 */

namespace {

	const std::vector<std::string_view> ALL_RUMBLE_NODES = { // used for body rumble
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
		"NPC L UpperarmTwist1 [LUt1]",
		"NPC L UpperarmTwist2 [LUt2]",
		"NPC L Forearm [LLar]",
		"NPC L ForearmTwist2 [LLt2]",
		"NPC L ForearmTwist1 [LLt1]",
		"NPC L Hand [LHnd]",
		"NPC R UpperarmTwist1 [RUt1]",
		"NPC R UpperarmTwist2 [RUt2]",
		"NPC R Forearm [RLar]",
		"NPC R ForearmTwist2 [RLt2]",
		"NPC R ForearmTwist1 [RLt1]",
		"NPC R Hand [RHnd]",
		"NPC L Breast",
		"NPC R Breast",
		"L Breast02",
		"R Breast02",
	};

	const std::vector<std::string_view> BODY_NODES = {
		"NPC R Thigh [RThg]",
		"NPC L Thigh [LThg]",
		"NPC R Butt",
		"NPC L Butt",
		"NPC Spine [Spn0]",
		"NPC Spine1 [Spn1]",
		"NPC Spine2 [Spn2]",
	};

	const std::string_view RNode = "NPC R Foot [Rft ]";
	const std::string_view LNode = "NPC L Foot [Lft ]";

	void StartRumble(std::string_view tag, Actor& actor, float power, float halflife) {
		for (auto& node_name: ALL_RUMBLE_NODES) {
			std::string rumbleName = std::format("BoobCrush_{}{}", tag, node_name);
			Rumbling::Start(rumbleName, &actor, power, halflife, node_name);
		}
	}

	void StopRumble(std::string_view tag, Actor& actor) {
		for (auto& node_name: ALL_RUMBLE_NODES) {
			std::string rumbleName = std::format("BoobCrush_{}{}", tag, node_name);
			Rumbling::Stop(rumbleName, &actor);
		}
	}

	void ModGrowthCount(Actor* giant, float value, bool reset) {
		auto transient = Transient::GetActorData(giant);
		if (transient) {
			transient->ButtCrushGrowthAmount += value;
			if (reset) {
				transient->ButtCrushGrowthAmount = 0.0f;
			}
		}
	}

	float GetGrowthCount(Actor* giant) {
		auto transient = Transient::GetActorData(giant);
		if (transient) {
			return transient->ButtCrushGrowthAmount;
		}
		return 1.0f;
	}

	void StartDamageOverTime(Actor* giant) {
		auto gianthandle = giant->CreateRefHandle();
		std::string name = std::format("BreastDOT_{}", giant->formID);
		
		float damage = AnimationBoobCrush::GetBoobCrushDamage(giant) * TimeScale();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giantref = gianthandle.get().get();

			auto BreastL = find_node(giantref, "NPC L Breast");
			auto BreastR = find_node(giantref, "NPC R Breast");
			auto BreastL02 = find_node(giantref, "L Breast02");
			auto BreastR02 = find_node(giantref, "R Breast02");

			if (!IsButtCrushing(giantref)) {
				return false;
			}

			float shake_power = Rumble_Cleavage_HoverLoop;

			for (auto Nodes: BODY_NODES) {
				auto Node = find_node(giantref, Nodes);
				if (Node) {
					std::string rumbleName = std::format("Node: {}", Nodes);
					DoDamageAtPoint(giant, Radius_BreastCrush_BodyDOT, Damage_BreastCrush_BodyDOT * damage, Node, 400, 0.10f, 1.33f, DamageSource::BodyCrush);
					Rumbling::Once(rumbleName, giant, shake_power, 0.02f, Nodes, 0.0f);
				}
			}

			if (BreastL02 && BreastR02) {
				Rumbling::Once("BreastDot_L", giantref, shake_power, 0.025f, "L Breast03", 0.0f);
				Rumbling::Once("BreastDot_R", giantref, shake_power, 0.025f, "R Breast03", 0.0f);
				DoDamageAtPoint(giant, Radius_BreastCrush_BreastDOT, Damage_BreastCrush_BreastDOT * damage, BreastL02, 400, 0.10f, 1.33f, DamageSource::BreastImpact);
				DoDamageAtPoint(giant, Radius_BreastCrush_BreastDOT, Damage_BreastCrush_BreastDOT * damage, BreastR02, 400, 0.10f, 1.33f, DamageSource::BreastImpact);
				return true;
			} else if (BreastL && BreastR) {
				Rumbling::Once("BreastDot_L", giantref, shake_power, 0.025f, "NPC L Breast", 0.0f);
				Rumbling::Once("BreastDot_R", giantref, shake_power, 0.025f, "NPC R Breast", 0.0f);
				DoDamageAtPoint(giant, Radius_BreastCrush_BreastDOT, Damage_BreastCrush_BreastDOT * damage, BreastL, 400, 0.10f, 1.33f, DamageSource::BreastImpact);
				DoDamageAtPoint(giant, Radius_BreastCrush_BreastDOT, Damage_BreastCrush_BreastDOT * damage, BreastR, 400, 0.10f, 1.33f, DamageSource::BreastImpact);
				return true;
			}
			return false;
		});

		TaskManager::ChangeUpdate(name, UpdateKind::Havok);
	}

	void StopDamageOverTime(Actor* giant) {
		std::string name = std::format("BreastDOT_{}", giant->formID);
		TaskManager::Cancel(name);
	}

	void LayingStaminaDrain_Launch(Actor* giant) {
		std::string name = std::format("LayingDrain_{}", giant->formID);
		auto gianthandle = giant->CreateRefHandle();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giantref = gianthandle.get().get();

			float stamina = GetAV(giantref, ActorValue::kStamina);
			DamageAV(giantref, ActorValue::kStamina, 0.12f * GetButtCrushCost(giant, false));

			if (!IsButtCrushing(giantref)) {
				return false;
			}
			return true;
		});
	}

	void LayingStaminaDrain_Cancel(Actor* giant) {
		std::string name = std::format("LayingDrain_{}", giant->formID);
		TaskManager::Cancel(name);
	}

	void InflictBodyDamage(Actor* giant) {
		float damage = AnimationBoobCrush::GetBoobCrushDamage(giant);
		float perk = GetPerkBonus_Basics(giant);
		for (auto Nodes: BODY_NODES) {
			auto Node = find_node(giant, Nodes);
			if (Node) {
				std::string rumbleName = std::format("Node: {}", Nodes);
				DoDamageAtPoint(giant, Radius_BreastCrush_BodyImpact, Damage_BreastCrush_Body * damage, Node, 400, 0.10f, 0.8f, DamageSource::BodyCrush);
				DoLaunch(giant, 2.25f * perk, 5.0f, Node);
				Rumbling::Once(rumbleName, giant, 1.00f * damage, 0.035f, Nodes, 0.0f);
			}
		}
	}

	void InflictBreastDamage(Actor* giant) {
		float damage = AnimationBoobCrush::GetBoobCrushDamage(giant);
		Actor* victim = AnimationBoobCrush::GetSingleton().GetBoobCrushVictim(giant);
		if (victim) {
			SetBeingEaten(victim, false); // Allow to be staggered
		}

		float perk = GetPerkBonus_Basics(giant);
		float dust = 1.0f;
		float smt = 1.0f;

		InflictBodyDamage(giant);

		if (HasSMT(giant)) {
			dust = 1.25f;
			smt = 1.5f;
		}

		auto BreastL = find_node(giant, "NPC L Breast");
		auto BreastR = find_node(giant, "NPC R Breast");
		auto BreastL02 = find_node(giant, "L Breast02");
		auto BreastR02 = find_node(giant, "R Breast02");

		float shake_power = Rumble_Cleavage_Impact/2 * dust * damage;

		if (BreastL02 && BreastR02) {
			DoDamageAtPoint(giant, Radius_BreastCrush_BreastImpact, Damage_BreastCrush_BreastImpact * damage, BreastL02, 4, 0.70f, 0.8f, DamageSource::BreastImpact);
			DoDamageAtPoint(giant, Radius_BreastCrush_BreastImpact, Damage_BreastCrush_BreastImpact * damage, BreastR02, 4, 0.70f, 0.8f, DamageSource::BreastImpact);
			DoDustExplosion(giant, 1.25f * dust + damage/10, FootEvent::Left, "L Breast03");
			DoDustExplosion(giant, 1.25f * dust + damage/10, FootEvent::Right, "R Breast03");
			DoFootstepSound(giant, 1.25f, FootEvent::Right, "R Breast03");
			DoFootstepSound(giant, 1.25f, FootEvent::Left, "L Breast03");
			DoLaunch(giant, 2.25f * perk, 5.0f, FootEvent::Breasts);
			Rumbling::Once("Breast_L", giant, shake_power * smt, 0.075f, "L Breast03", 0.0f);
			Rumbling::Once("Breast_R", giant, shake_power * smt, 0.075f, "R Breast03", 0.0f);
			ModGrowthCount(giant, 0, true); // Reset limit
		} else if (BreastL && BreastR) {
			DoDamageAtPoint(giant, Radius_BreastCrush_BreastImpact, Damage_BreastCrush_BreastImpact * damage, BreastL, 4, 0.70f, 0.8f, DamageSource::BreastImpact);
			DoDamageAtPoint(giant, Radius_BreastCrush_BreastImpact, Damage_BreastCrush_BreastImpact * damage, BreastR, 4, 0.70f, 0.8f, DamageSource::BreastImpact);
			DoDustExplosion(giant, 1.25f * dust + damage/10, FootEvent::Left, "NPC L Breast");
			DoDustExplosion(giant, 1.25f * dust + damage/10, FootEvent::Right, "NPC R Breast");
			DoFootstepSound(giant, 1.25f, FootEvent::Right, "NPC R Breast");
			DoFootstepSound(giant, 1.25f, FootEvent::Right, "NPC L Breast");
			DoLaunch(giant, 2.25f * perk, 5.0f, FootEvent::Breasts);
			Rumbling::Once("Breast_L", giant, shake_power * smt, 0.075f, "NPC L Breast", 0.0f);
			Rumbling::Once("Breast_R", giant, shake_power * smt, 0.075f, "NPC R Breast", 0.0f);
			ModGrowthCount(giant, 0, true); // Reset limit
		} else {
			if (!BreastR) {
				Notify("Error: Missing Breast Nodes"); // Will help people to troubleshoot it. Not everyone has 3BB/XPMS32 body.
				Notify("Error: effects not inflicted");
				Notify("Suggestion: install Female body replacer");
			} else if (!BreastR02) {
				Notify("Error: Missing 3BB Breast Nodes"); // Will help people to troubleshoot it. Not everyone has 3BB/XPMS32 body.
				Notify("Error: effects not inflicted");
				Notify("Suggestion: install 3BB/SMP Body");
			}
		}
	}

	void GTS_BoobCrush_Smile_On(AnimationEventData& data) {
		auto giant = &data.giant;
		AdjustFacialExpression(giant, 0, 1.0f, CharEmotionType::Modifier); // blink L
		AdjustFacialExpression(giant, 1, 1.0f, CharEmotionType::Modifier); // blink R
		AdjustFacialExpression(giant, 2, 1.0f, CharEmotionType::Expression);

		ApplyButtCrushCooldownTask(giant);
		//AdjustFacialExpression(giant, 0, 0.75f, CharEmotionType::Phenome);
	}

	void GTS_BoobCrush_Smile_Off(AnimationEventData& data) {
		auto giant = &data.giant;
		AdjustFacialExpression(giant, 0, 0.0f, CharEmotionType::Modifier); // blink L
		AdjustFacialExpression(giant, 1, 0.0f, CharEmotionType::Modifier); // blink R
		AdjustFacialExpression(giant, 2, 0.0f, CharEmotionType::Expression);
		//AdjustFacialExpression(giant, 0, 0.0f, CharEmotionType::Phenome);
	}

	void GTS_BoobCrush_DOT_Start_Loop(AnimationEventData& data) {
		StartDamageOverTime(&data.giant);
	}

	void GTS_BoobCrush_TrackBody(AnimationEventData& data) {
		RecordStartButtCrushSize(&data.giant);
		ManageCamera(&data.giant, true, CameraTracking::ObjectB);
	}
	void GTS_BoobCrush_UnTrackBody(AnimationEventData& data) {
		ManageCamera(&data.giant, false, CameraTracking::ObjectB);
	}
	void GTS_BoobCrush_BreastImpact(AnimationEventData& data) {
		InflictBreastDamage(&data.giant);
	}
	void GTS_BoobCrush_DOT_Start(AnimationEventData& data) {
		LayingStaminaDrain_Launch(&data.giant);
	}
	void GTS_BoobCrush_DOT_End(AnimationEventData& data) {
		auto giant = &data.giant;
		StopDamageOverTime(giant);
		ModGrowthCount(giant, 0, true);
		LayingStaminaDrain_Cancel(giant);
	}
	void GTS_BoobCrush_Grow_Start(AnimationEventData& data) {
		auto giant = &data.giant;

		float gigantism = 1.0f + (Ench_Aspect_GetPower(giant) / 5);
		float bonus = 0.24f * gigantism * (GetGrowthCount(giant) + 1.0f);

		Sound_PlayMoans(giant, 1.0f, 0.14f, EmotionTriggerSource::Growth, CooldownSource::Emotion_Voice_Long);
		ModGrowthCount(giant, 1.0f, false);
		SetButtCrushSize(giant, bonus, false);
		SpringGrow(giant, bonus, 0.3f / GetAnimationSlowdown(giant), "BreastCrushGrowth", false);

		float WasteStamina = 100.0f * GetButtCrushCost(giant, false);
		DamageAV(giant, ActorValue::kStamina, WasteStamina);
		
		Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundGrowth, giant, 1.0f, "NPC Pelvis [Pelv]");
		Task_FacialEmotionTask_Moan(giant, 0.675f, "BoobCrush_Growth", 0.20f);
		
		StartRumble("CleavageRumble", data.giant, 0.06f, 0.60f);
	}
	void GTS_BoobCrush_Grow_Stop(AnimationEventData& data) {
		StopRumble("CleavageRumble", data.giant);
	}

	void GTS_BoobCrush_LoseSize(AnimationEventData& data) {
		SetButtCrushSize(&data.giant, 0.0f, true);
	}
}

namespace GTS
{	

	AnimationBoobCrush& AnimationBoobCrush::GetSingleton() noexcept {
		static AnimationBoobCrush instance;
		return instance;
	}

	std::string AnimationBoobCrush::DebugName() {
		return "::AnimationBoobCrush";
	}

	void AnimationBoobCrush::AttachActor(Actor* giant, Actor* tiny) {
		AnimationBoobCrush::GetSingleton().data.try_emplace(giant, tiny);
	}

	void AnimationBoobCrush::Reset() {
		this->data.clear();
	}

	void AnimationBoobCrush::ResetActor(Actor* actor) {
		this->data.erase(actor);
	}

	Actor* AnimationBoobCrush::GetBoobCrushVictim(Actor* giant) {
		try {
			auto& me = AnimationBoobCrush::GetSingleton();

			if (me.data.empty() || !me.data.contains(giant)) {
				return nullptr;
			}

			return me.data.at(giant).tiny;
		}
		catch (const std::out_of_range&) {
			return nullptr;
		}
	}

	float AnimationBoobCrush::GetBoobCrushDamage(Actor* actor) {
		float damage = 1.0f;
		if (Runtime::HasPerkTeam(actor, Runtime::PERK.GTSPerkButtCrush)) {
			damage += 0.30f;
		}
		if (Runtime::HasPerkTeam(actor, Runtime::PERK.GTSPerkButtCrushAug3)) {
			damage += 0.70f;
		}
		return damage;
	}

	void AnimationBoobCrush::RegisterEvents() {
		AnimationManager::RegisterEvent("GTS_BoobCrush_DOT_Start_Loop", "BoobCrush", GTS_BoobCrush_DOT_Start_Loop);
		AnimationManager::RegisterEvent("GTS_BoobCrush_Smile_On", "BoobCrush", GTS_BoobCrush_Smile_On);
		AnimationManager::RegisterEvent("GTS_BoobCrush_Smile_Off", "BoobCrush", GTS_BoobCrush_Smile_Off);
		AnimationManager::RegisterEvent("GTS_BoobCrush_TrackBody", "BoobCrush", GTS_BoobCrush_TrackBody);
		AnimationManager::RegisterEvent("GTS_BoobCrush_UnTrackBody", "BoobCrush", GTS_BoobCrush_UnTrackBody);
		AnimationManager::RegisterEvent("GTS_BoobCrush_BreastImpact", "BoobCrush", GTS_BoobCrush_BreastImpact);
		AnimationManager::RegisterEvent("GTS_BoobCrush_DOT_Start", "BoobCrush", GTS_BoobCrush_DOT_Start);
		AnimationManager::RegisterEvent("GTS_BoobCrush_DOT_End", "BoobCrush", GTS_BoobCrush_DOT_End);
		AnimationManager::RegisterEvent("GTS_BoobCrush_Grow_Start", "BoobCrush", GTS_BoobCrush_Grow_Start);
		AnimationManager::RegisterEvent("GTS_BoobCrush_Grow_Stop", "BoobCrush", GTS_BoobCrush_Grow_Stop);
		AnimationManager::RegisterEvent("GTS_BoobCrush_LoseSize", "BoobCrush", GTS_BoobCrush_LoseSize);
	}

	BoobCrushData::BoobCrushData(Actor* tiny) : tiny(tiny) {
	}
}
