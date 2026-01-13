#include "Managers/Animation/ThighCrush.hpp"
#include "Managers/Animation/AnimationManager.hpp"

#include "Managers/Animation/Controllers/ThighCrushController.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CrawlUtils.hpp"

#include "Managers/Input/InputManager.hpp"
#include "Managers/Audio/Footstep.hpp"
#include "Managers/AI/AIFunctions.hpp"
#include "Managers/Rumble.hpp"

#include "Magic/Effects/Common.hpp"

#include "Utils/Actions/InputConditions.hpp"

using namespace GTS;

// Animation: ThighCrush
//  - Stages
//    - "GTStosit",                     // [0] Start air rumble and camera shake
//    - "GTSsitloopenter",              // [1] Sit down completed
//    - "GTSsitloopstart",              // [2] enter sit crush loop
//    - "GTSsitloopend",                // [3] unused
//    - "GTSsitcrushlight_start",       // [4] Start Spreading legs
//    - "GTSsitcrushlight_end",         // [5] Legs fully spread
//    - "GTSsitcrushheavy_start",       // [6] Start Closing legs together
//    - "GTSsitcrushheavy_end",         // [7] Legs fully closed
//    - "GTSsitloopexit",               // [8] stand up, small air rumble and camera shake
//    - "GTSstandR",                    // [9] feet collides with ground when standing up
//    - "GTSstandL",                    // [10]
//    - "GTSstandRS",                   // [11] Silent impact of right feet
//    - "GTStoexit",                    // [12] Leave animation, disable air rumble and such

namespace {

	constexpr std::string_view RNode = "NPC R Foot [Rft ]";
	constexpr std::string_view LNode = "NPC L Foot [Lft ]";

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

	const std::vector<std::string_view> LEG_RUMBLE_NODES = { // used with Anim_ThighCrush
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

	void AdjustAnimationSpeed(Actor* giant, AnimationEventData& data, float force, int range, bool reset) {
		// Works on non-player only
		if (giant->formID != 0x14) {
			if (reset) {
				data.animSpeed = 1.0f;
			} else {
				int rng = 100 + (RandomInt(0, range));
				data.animSpeed = (rng/100 * force);
			}
		}
	}

	void FreezeTinies(Actor* giant, float duration) {
		std::vector<Actor*> tinies = ThighCrushController::GetSingleton().GetThighTargetsInFront(giant, 1000);
		if (!tinies.empty()) {
			for (auto tiny: tinies) {
				if (tiny) {
					if (tiny->formID != 0x14 && !IsTeammate(tiny)) {
						ForceFlee(giant, tiny, duration, false);
					}
				}
			}
		}
	}

	void LegRumblingOnce(std::string_view tag, Actor& actor, float power, float halflife) {
		for (auto& node_name: LEG_RUMBLE_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			Rumbling::Once(rumbleName, &actor, power, halflife, node_name, 0.0f);
		}
	}

	void StartLegRumbling(std::string_view tag, Actor& actor, float power, float halflife) {
		for (auto& node_name: LEG_RUMBLE_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			Rumbling::Start(rumbleName, &actor, power / LEG_RUMBLE_NODES.size(),  halflife, node_name);
		}
	}

	void StartBodyRumble(std::string_view tag, Actor& actor, float power, float halflife) {
		for (auto& node_name: BODY_RUMBLE_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			Rumbling::Start(rumbleName, &actor, power / BODY_RUMBLE_NODES.size(),  halflife, node_name);
		}
	}

	void StopLegRumbling(std::string_view tag, Actor& actor) {
		for (auto& node_name: LEG_RUMBLE_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			Rumbling::Stop(rumbleName, &actor);
		}
	}

	void RunThighCollisionTask(Actor* giant, bool right, bool CooldownCheck, float radius, float damage, float bbmult, float crush_threshold, int random, std::string_view taskname) {
		std::string name = std::format("ThighCrush_{}_{}", giant->formID, taskname);
		auto gianthandle = giant->CreateRefHandle();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giantref = gianthandle.get().get();

			if (!AnimationVars::Action::IsThighCrushing(giantref)) {
				return false; //Disable it once we leave Thigh Crush state
			}
			float animspeed = AnimationManager::GetBonusAnimationSpeed(giantref);
			ApplyThighDamage(giant, right, CooldownCheck, radius, damage * animspeed, bbmult, crush_threshold, random, DamageSource::ThighCrushed);

			return true; // Cancel it
		});
	}

	void RunButtCollisionTask(Actor* giant) {
		std::string name = std::format("ButtCrush_{}", giant->formID);
		auto gianthandle = giant->CreateRefHandle();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giantref = gianthandle.get().get();
			auto ThighL = find_node(giantref, "NPC L Thigh [LThg]");
			auto ThighR = find_node(giantref, "NPC R Thigh [RThg]");

			if (!AnimationVars::Action::IsThighCrushing(giantref)) {
				return false; //Disable it once we leave Thigh Crush state
			}
			if (ThighL && ThighR) {
				DoDamageAtPoint(giantref, Radius_ThighCrush_Butt_DOT, Damage_ThighCrush_Butt_DOT * TimeScale(), ThighL, 100, 0.20f, 2.5f, DamageSource::Booty);
				DoDamageAtPoint(giantref, Radius_ThighCrush_Butt_DOT, Damage_ThighCrush_Butt_DOT * TimeScale(), ThighR, 100, 0.20f, 2.5f, DamageSource::Booty);
				return true;
			}
			return false; // Cancel it if we don't have these bones
		});
	}

	void ThighCrush_GetUpFootstepDamage(Actor* giant, bool right, float animSpeed, float mult, FootEvent Event, DamageSource Source, std::string_view Node, std::string_view rumble) {
		float getup = 1.0f;
		float speed = animSpeed;
		float scale = get_visual_scale(giant);
		float volume = scale * 0.10f * speed;
		float perk = GetPerkBonus_Thighs(giant);

		float shake_power = Rumble_ThighCrush_StandUp * mult * GetHighHeelsBonusDamage(giant, true);

		if (HasSMT(giant)) {
			shake_power = 2.0f;
		}
		
		DoDamageEffect(giant, Damage_ThighCrush_Stand_Up * mult * perk, Radius_ThighCrush_Stand_Up, 25, 0.20f, Event, 1.0f, Source);
		DoLaunch(giant, 0.65f * mult * perk, 2.0f * animSpeed, Event);
		Rumbling::Once(rumble, giant, shake_power, 0.10f, Node, 0.0f);
		DoFootstepSound(giant, 1.0f, Event, Node);
		DoDustExplosion(giant, 1.0f, Event, Node);
	}

	////////////////////////////////////////////////////////////////////////////////////
	//// EVENTS
	///////////////////////////////////////////////////////////////////////////////////

	void GTStosit(AnimationEventData& data) {
		float speed = data.animSpeed;
		StartLegRumbling("ThighCrush", data.giant, 1.35f, 0.10f);
		ManageCamera(&data.giant, true, CameraTracking::Thigh_Crush); // Track feet

		RunThighCollisionTask(&data.giant, true, false, Radius_ThighCrush_Idle, Damage_ThighCrush_Legs_Idle, 0.02f, 2.0f, 600, "ThighIdle_R");
		RunThighCollisionTask(&data.giant, false, false, Radius_ThighCrush_Idle, Damage_ThighCrush_Legs_Idle, 0.02f, 2.0f, 600, "ThighIdle_L");

		RunButtCollisionTask(&data.giant);

		FreezeTinies(&data.giant, 2.8f / AnimationManager::GetAnimSpeed(&data.giant));

		data.stage = 1;
	}

	void GTSsitloopenter(AnimationEventData& data) {
		float speed = data.animSpeed;
		StartLegRumbling("ThighCrush", data.giant, 1.25f * speed, 0.10f);
		data.disableHH = true;
		data.HHspeed = 4.0f;
		data.stage = 2;
	}

	void GTSsitloopstart(AnimationEventData& data) {
		StopLegRumbling("ThighCrush", data.giant);
		data.currentTrigger = 1;
		data.stage = 3;
	}

	void GTSsitloopend(AnimationEventData& data) {
		StopLegRumbling("ThighCrush", data.giant);
		data.stage = 4;
	}

	void GTSsitcrushlight_start(AnimationEventData& data) {
		auto giant = &data.giant;
		StartLegRumbling("ThighCrush", data.giant, Rumble_ThighCrush_LegSpread_Light_Loop, 0.12f);
		DrainStamina(&data.giant, "StaminaDrain_Thighs", Runtime::PERK.GTSPerkThighAbilities, true, 1.0f); // < Start Light Stamina Drain

		std::string name_l = std::format("ThighCrush_{}_ThighIdle_R", giant->formID);
		std::string name_r = std::format("ThighCrush_{}_ThighIdle_L", giant->formID);
		TaskManager::Cancel(name_l);
		TaskManager::Cancel(name_r);

		RunThighCollisionTask(&data.giant, true, true, Radius_ThighCrush_Spread_Out, Damage_ThighCrush_CrossLegs_Out, 0.10f, 1.70f, 50, "ThighLight_R");
		RunThighCollisionTask(&data.giant, false, true, Radius_ThighCrush_Spread_Out, Damage_ThighCrush_CrossLegs_Out, 0.10f, 1.70f, 50, "ThighLight_L");
		
		AdjustAnimationSpeed(giant, data, 1.0f, 85, false);

		data.stage = 5;
	}

	void GTSsitcrushlight_end(AnimationEventData& data) {
		auto giant = &data.giant;
		data.currentTrigger = 2;
		data.canEditAnimSpeed = true;
		StopLegRumbling("ThighCrush", data.giant);
		LegRumblingOnce("ThighCrush_End", data.giant, Rumble_ThighCrush_LegSpread_Light_End, 0.10f);

		DrainStamina(&data.giant, "StaminaDrain_Thighs", Runtime::PERK.GTSPerkThighAbilities, false, 1.0f); // < Stop Light Stamina Drain

		std::string name_l = std::format("ThighCrush_{}_ThighLight_R", giant->formID);
		std::string name_r = std::format("ThighCrush_{}_ThighLight_L", giant->formID);
		TaskManager::Cancel(name_l);
		TaskManager::Cancel(name_r);

		AdjustAnimationSpeed(giant, data, 1.0f, 85, true);

		data.stage = 6;
	}

	void GTSsitcrushheavy_start(AnimationEventData& data) {
		auto giant = &data.giant;
		DrainStamina(&data.giant, "StaminaDrain_Thighs", Runtime::PERK.GTSPerkThighAbilities, true, 2.5f); // < - Start HEAVY Stamina Drain

		std::string name_l = std::format("ThighCrush_{}_ThighIdle_R", giant->formID);
		std::string name_r = std::format("ThighCrush_{}_ThighIdle_L", giant->formID);
		TaskManager::Cancel(name_l);
		TaskManager::Cancel(name_r);

		RunThighCollisionTask(&data.giant, true, true, Radius_ThighCrush_Spread_In, Damage_ThighCrush_CrossLegs_In, 0.25f, 1.4f, 25, "ThighHeavy_R");
		RunThighCollisionTask(&data.giant, false, true, Radius_ThighCrush_Spread_In, Damage_ThighCrush_CrossLegs_In, 0.25f, 1.4f, 25, "ThighHeavy_L");

		StartLegRumbling("ThighCrushHeavy", data.giant, Rumble_ThighCrush_LegSpread_Heavy_Loop, 0.10f);

		AdjustAnimationSpeed(giant, data, 1.5f, 125, false);

		data.stage = 5;
	}

	void GTSsitcrushheavy_end(AnimationEventData& data) {
		auto giant = &data.giant;
		data.currentTrigger = 2;
		DrainStamina(&data.giant, "StaminaDrain_Thighs", Runtime::PERK.GTSPerkThighAbilities, false, 2.5f); // < Stop Heavy Stamina Drain

		StopLegRumbling("ThighCrushHeavy", data.giant);
		LegRumblingOnce("ThighCrushHeavy_End", data.giant, Rumble_ThighCrush_LegCross_Heavy_End, 0.10f);
		

		std::string name_l = std::format("ThighCrush_{}_ThighHeavy_R", giant->formID);
		std::string name_r = std::format("ThighCrush_{}_ThighHeavy_L", giant->formID);
		TaskManager::Cancel(name_l);
		TaskManager::Cancel(name_r);

		RunThighCollisionTask(&data.giant, true, false, Radius_ThighCrush_Idle, Damage_ThighCrush_Legs_Idle, 0.02f, 3.0f, 600, "ThighIdle_R");
		RunThighCollisionTask(&data.giant, false, false, Radius_ThighCrush_Idle, Damage_ThighCrush_Legs_Idle, 0.02f, 3.0f, 600, "ThighIdle_L");

		AdjustAnimationSpeed(giant, data, 1.0f, 125, true);

		data.stage = 6;
	}

	void GTSsitloopexit(AnimationEventData& data) {
		float scale = get_visual_scale(data.giant);
		float speed = data.animSpeed;

		data.HHspeed = 1.0f;
		data.disableHH = false;
		data.canEditAnimSpeed = false;
		data.animSpeed = 1.0f;

		StartBodyRumble("BodyRumble", data.giant, 0.25f, 0.12f);
		data.stage = 8;
	}

	void GTSstandR(AnimationEventData& data) {
		// do stand up damage
		data.stage += 1;
		if (data.stage <= 10) { // fix for double footsteps
			ThighCrush_GetUpFootstepDamage(&data.giant, true, data.animSpeed, 1.0f, FootEvent::Right, DamageSource::CrushedRight, RNode, "ThighCrushStompR");
			FootStepManager::PlayVanillaFootstepSounds(&data.giant, true);
		}
	}

	void GTSstandL(AnimationEventData& data) {
		// do stand up damage
		ThighCrush_GetUpFootstepDamage(&data.giant, false, data.animSpeed, 1.0f, FootEvent::Left, DamageSource::CrushedLeft, LNode, "ThighCrushStompL");
		FootStepManager::PlayVanillaFootstepSounds(&data.giant, false);
		data.stage = 9;
	}

	void GTSstandRS(AnimationEventData& data) {
		// do weaker stand up damage
		ThighCrush_GetUpFootstepDamage(&data.giant, true, data.animSpeed, 0.8f, FootEvent::Right, DamageSource::CrushedRight, RNode, "ThighCrushStompR");
		data.stage = 9;
	}
	void GTSBEH_Next(AnimationEventData& data) {
		data.animSpeed = 1.0f;
		data.canEditAnimSpeed = false;
	}
	void GTStoexit(AnimationEventData& data) {
		// Going to exit
		StopLegRumbling("BodyRumble", data.giant);
		ManageCamera(&data.giant, false, CameraTracking::Thigh_Crush); // Un-track feet
	}
	void GTSBEH_Exit(AnimationEventData& data) {
		// Final exit
		data.stage = 0;
	}

	void ThighCrushEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		AnimationManager::StartAnim("ThighLoopEnter", player);
	}

	void ThighCrushKillEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		if (AnimationVars::General::IsGTSBusy(player)) {
			float WasteStamina = 40.0f;
			if (Runtime::HasPerk(player, Runtime::PERK.GTSPerkThighAbilities)) {
				WasteStamina *= 0.65f;
			}
			if (GetAV(player, ActorValue::kStamina) > WasteStamina) {
				AnimationManager::StartAnim("ThighLoopAttack", player);
			} else {
				if (AnimationVars::Action::IsThighCrushing(player)) {
					NotifyWithSound(player, "You're too tired to perform thighs attack");
				}
			}
		}
	}

	void ThighCrushSpareEvent(const ManagedInputEvent& data) {
		if (!IsFreeCameraEnabled()) {
			auto player = PlayerCharacter::GetSingleton();
			if (AnimationVars::General::IsGTSBusy(player)) {
				AnimationManager::StartAnim("ThighLoopExit", player);
			}
		}
	}
}

namespace GTS
{
	void AnimationThighCrush::RegisterEvents() {
		AnimationManager::RegisterEvent("GTStosit", "ThighCrush", GTStosit);
		AnimationManager::RegisterEvent("GTSsitloopenter", "ThighCrush", GTSsitloopenter);
		AnimationManager::RegisterEvent("GTSsitloopstart", "ThighCrush", GTSsitloopstart);
		AnimationManager::RegisterEvent("GTSsitloopend", "ThighCrush", GTSsitloopend);
		AnimationManager::RegisterEvent("GTSsitcrushlight_start", "ThighCrush", GTSsitcrushlight_start);
		AnimationManager::RegisterEvent("GTSsitcrushlight_end", "ThighCrush", GTSsitcrushlight_end);
		AnimationManager::RegisterEvent("GTSsitcrushheavy_start", "ThighCrush", GTSsitcrushheavy_start);
		AnimationManager::RegisterEvent("GTSsitcrushheavy_end", "ThighCrush", GTSsitcrushheavy_end);
		AnimationManager::RegisterEvent("GTSsitloopexit", "ThighCrush", GTSsitloopexit);
		AnimationManager::RegisterEvent("GTSstandR", "ThighCrush", GTSstandR);
		AnimationManager::RegisterEvent("GTSstandL", "ThighCrush", GTSstandL);
		AnimationManager::RegisterEvent("GTSstandRS", "ThighCrush", GTSstandRS);
		AnimationManager::RegisterEvent("GTStoexit", "ThighCrush", GTStoexit);
		AnimationManager::RegisterEvent("GTSBEH_Next", "ThighCrush", GTSBEH_Next);
		AnimationManager::RegisterEvent("GTSBEH_Exit", "ThighCrush", GTSBEH_Exit);

		InputManager::RegisterInputEvent("ThighCrush", ThighCrushEvent, ThighCrushCondition_Start);
		InputManager::RegisterInputEvent("ThighCrushKill", ThighCrushKillEvent);
		InputManager::RegisterInputEvent("ThighCrushSpare", ThighCrushSpareEvent);
	}

	void AnimationThighCrush::RegisterTriggers() {
		AnimationManager::RegisterTriggerWithStages("ThighCrush", "ThighCrush", {"GTSBeh_TriggerSitdown", "GTSBeh_StartThighCrush", "GTSBeh_LeaveSitdown"});
		AnimationManager::RegisterTrigger("ThighLoopEnter", "ThighCrush", "GTSBeh_TriggerSitdown");
		AnimationManager::RegisterTrigger("ThighLoopAttack", "ThighCrush", "GTSBeh_StartThighCrush");
		AnimationManager::RegisterTrigger("ThighLoopExit", "ThighCrush", "GTSBeh_LeaveSitdown");
		AnimationManager::RegisterTrigger("ThighLoopFull", "ThighCrush", "GTSBeh_ThighAnimationFull");
	}
}
