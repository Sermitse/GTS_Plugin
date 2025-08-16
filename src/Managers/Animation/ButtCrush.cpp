
#include "Managers/Animation/Controllers/ButtCrushController.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CrawlUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/ButtCrush.hpp"

#include "Managers/Audio/Footstep.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Managers/Rumble.hpp"
#include "Utils/ButtCrushUtils.hpp"
#include "Utils/InputConditions.hpp"

#include "Managers/Audio/MoansLaughs.hpp"

using namespace GTS;

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
	};

	constexpr std::string_view RNode = "NPC R Foot [Rft ]";
	constexpr std::string_view LNode = "NPC L Foot [Lft ]";

	void StartRumble(std::string_view tag, Actor& actor, float power, float halflife) {
		for (auto& node_name: ALL_RUMBLE_NODES) {
			std::string rumbleName = std::format("ButtCrush_{}{}", tag, node_name);
			Rumbling::Start(rumbleName, &actor, power / ALL_RUMBLE_NODES.size(), halflife, node_name);
		}
	}

	void StopRumble(std::string_view tag, Actor& actor) {
		for (auto& node_name: ALL_RUMBLE_NODES) {
			std::string rumbleName = std::format("ButtCrush_{}{}", tag, node_name);
			Rumbling::Stop(rumbleName, &actor);
		}
	}

	void DisableButtTrackTask(Actor* giant) {
		std::string name = std::format("DisableCameraTask_{}", giant->formID);
		auto gianthandle = giant->CreateRefHandle();
		double Start = Time::WorldTimeElapsed();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giantref = gianthandle.get().get();

			double Finish = Time::WorldTimeElapsed();
			if ((Finish - Start) / AnimationManager::GetAnimSpeed(giantref) > 12.0f || !IsButtCrushing(giantref) && !IsGtsBusy(giantref)) {
				ManageCamera(giantref, false, CameraTracking::Butt);
				return false;
			}
			
			return true;
		});
	}

	void ButtCrush_DoFootImpact(Actor* giant, bool right, FootEvent Event, DamageSource Source, std::string_view Node, std::string_view rumble) {
		float perk = GetPerkBonus_Basics(giant);
		float smt = 1.0f;
		float dust = 1.0f;
		if (HasSMT(giant)) {
			dust = 1.25f;
			smt = 1.5f;
		}

		float shake_power = Rumble_ButtCrush_FeetImpact * smt * GetHighHeelsBonusDamage(giant, true);

		Rumbling::Once(rumble, giant, shake_power, 0.0f, Node, 0.33f);
		DoDamageEffect(giant, Damage_ButtCrush_FootImpact, Radius_ButtCrush_FootImpact, 10, 0.25f, Event, 1.0f, Source);
		DoFootstepSound(giant, 1.0f, Event, Node);
		DoDustExplosion(giant, dust, Event, Node);
		DoLaunch(giant, 0.75f * perk, 1.6f, Event);

		FootStepManager::PlayVanillaFootstepSounds(giant, right);
	}

	/////////////////////////////////////////////////////////////////////
	/// EVENTS
	/////////////////////////////////////////////////////////////////////

	

	void GTSButtCrush_MoveBody_MixFrameToLoop(AnimationEventData& data) {
		Actor* giant = &data.giant;
		ManageCamera(giant, true, CameraTracking::ObjectB);
	}
	void GTSButtCrush_MoveBody_Start(AnimationEventData& data) {
		ApplyButtCrushCooldownTask(&data.giant);
		RecordStartButtCrushSize(&data.giant);

		ManageCamera(&data.giant, true, CameraTracking::Butt);
	}
	void GTSButtCrush_MoveBody_Stop(AnimationEventData& data) { // When doing quick butt crush
		ManageCamera(&data.giant, true, CameraTracking::Butt);
	}

	void GTSButtCrush_GrowthStart(AnimationEventData& data) {
		auto giant = &data.giant;

		float scale = get_visual_scale(giant);
		float gigantism = 1.0f + (Ench_Aspect_GetPower(giant) / 5);
		float bonus = 0.24f * gigantism * (GetGrowthCount(giant) + 1.0f);

		ModGrowthCount(giant, 1.0f, false);
		SetButtCrushSize(giant, bonus, false);
		SpringGrow(giant, bonus, 0.3f / GetAnimationSlowdown(giant), "ButtCrushGrowth", false);

		float WasteStamina = 100.0f * GetButtCrushCost(giant, false);

		if (giant->formID != 0x14) {
			WasteStamina *= 0.25f;
		}
		
		DamageAV(giant, ActorValue::kStamina, WasteStamina);

		Runtime::PlaySoundAtNode("GTSSoundGrowth", giant, 1.0f, "NPC Pelvis [Pelv]");

		StartRumble("BCRumble", data.giant, 1.25f, 0.30f);
	}

	void GTSBEH_ButtCrush_GrowthFinish(AnimationEventData& data) {
		auto* giant = &data.giant;
		for (auto tiny: find_actors()) {
			if (tiny && tiny != giant) {
				if (IsHostile(giant, tiny) || IsHostile(tiny, giant)) {
					NiPoint3 distance_a = giant->GetPosition();
					NiPoint3 distance_b = tiny->GetPosition();
					float distance = (distance_a - distance_b).Length();
					if (distance <= 212 * get_visual_scale(giant)) {
						ChanceToScare(giant, tiny, 1, 30, true);
					}
				}
			}
		}

		bool Blocked = IsActionOnCooldown(giant, CooldownSource::Emotion_Moan);
		if (!Blocked) {
			Sound_PlayMoans(giant, 1.0f, 0.14f, EmotionTriggerSource::Growth, CooldownSource::Emotion_Voice_Long);
			ApplyActionCooldown(giant, CooldownSource::Emotion_Moan);
			Task_FacialEmotionTask_Moan(giant, 1.2f, "ButtCrush_Growth");
		}

		StopRumble("BCRumble", data.giant);
	}

	void GTSButtCrush_FootstepR(AnimationEventData& data) {
		// do footsteps
		//Rumbling::Stop("FS_L", &data.giant);
		ButtCrush_DoFootImpact(&data.giant, true, FootEvent::Right, DamageSource::CrushedRight, RNode, "FS_L");
	}

	void GTSButtCrush_FootstepL(AnimationEventData& data) {
		// do footsteps
		//Rumbling::Stop("FS_R", &data.giant);
		ButtCrush_DoFootImpact(&data.giant, false, FootEvent::Left, DamageSource::CrushedLeft, LNode, "FS_L");
	}

	void GTSButtCrush_HandImpactR(AnimationEventData& data) {
		auto giant = &data.giant;
		float scale = get_visual_scale(giant);
		DoCrawlingFunctions(giant, scale, 1.0f, Damage_ButtCrush_HandImpact, CrawlEvent::RightHand, "RightHand", 0.8f, Radius_ButtCrush_HandImpact, 1.0f, DamageSource::HandCrawlRight);
		data.disableHH = false;
		data.HHspeed = 1.5f;
	}

	void GTSButtCrush_FallDownStart(AnimationEventData& data) {
		data.stage = 1;
		data.disableHH = true;
		data.HHspeed = 0.5f;
	}

	void GTSButtCrush_FallDownImpact(AnimationEventData& data) {
		auto giant = &data.giant;

		float perk = GetPerkBonus_Basics(&data.giant);
		float dust = 1.0f;
		float smt = 1.0f;

		if (HasSMT(giant)) {
			dust = 1.25f;
			smt = 1.5f;
		}
		std::string taskname = std::format("ButtCrushAttack_{}", giant->formID);
		ActorHandle giantHandle = giant->CreateRefHandle();

		ManageCamera(giant, true, CameraTracking::Butt);

		double Start = Time::WorldTimeElapsed();
		
		TaskManager::RunFor(taskname, 1.0f, [=](auto& update){ // Needed because anim has wrong timing
			if (!giantHandle) {
				return false;
			}

			double Finish = Time::WorldTimeElapsed();
			auto giantref = giantHandle.get().get();

			if (Finish - Start > 0.04) { 

				SetButtCrushSize(giant, 0.0f, true);

				float damage = GetButtCrushDamage(giantref);
				auto ThighL = find_node(giantref, "NPC L Thigh [LThg]");
				auto ThighR = find_node(giantref, "NPC R Thigh [RThg]");
				auto ButtR = find_node(giantref, "NPC R Butt");
				auto ButtL = find_node(giantref, "NPC L Butt");

				ApplyThighDamage(giantref, true, false, Radius_ThighCrush_ButtCrush_Drop, Damage_ButtCrush_LegDrop * damage, 0.35f, 1.0f, 14, DamageSource::Booty);
				ApplyThighDamage(giant, false, false, Radius_ThighCrush_ButtCrush_Drop, Damage_ButtCrush_LegDrop * damage, 0.35f, 1.0f, 14, DamageSource::Booty);

				float shake_power = Rumble_ButtCrush_ButtImpact/2 * dust * damage;

				if (ButtR && ButtL) {
					if (ThighL && ThighR) {
						DoDamageAtPoint(giantref, Radius_ButtCrush_Impact, Damage_ButtCrush_ButtImpact * damage, ThighL, 4, 0.70f, 0.8f, DamageSource::Booty);
						DoDamageAtPoint(giantref, Radius_ButtCrush_Impact, Damage_ButtCrush_ButtImpact * damage, ThighR, 4, 0.70f, 0.8f, DamageSource::Booty);
						DoDustExplosion(giantref, 1.45f * dust * damage, FootEvent::Butt, "NPC R Butt");
						DoDustExplosion(giantref, 1.45f * dust * damage, FootEvent::Butt, "NPC L Butt");
						DoLaunch(giantref, 2.25f * perk, 5.0f, FootEvent::Butt);
						DoFootstepSound(giantref, 1.25f, FootEvent::Right, RNode);
						
						Rumbling::Once("Butt_L", giantref, shake_power * smt, 0.075f, "NPC R Butt", 0.0f);
						Rumbling::Once("Butt_R", giantref, shake_power * smt, 0.075f, "NPC L Butt", 0.0f);
					}
				} else {
					if (!ButtR) {
						Notify("Error: Missing Butt Nodes"); // Will help people to troubleshoot it. Not everyone has 3BB/XPMS32 body.
						Notify("Error: effects not inflicted");
						Notify("install 3BBB/XP32 Skeleton");
					}
					if (!ThighL) {
						Notify("Error: Missing Thigh Nodes");
						Notify("Error: effects not inflicted");
						Notify("install 3BBB/XP32 Skeleton");
					}
				}
				ModGrowthCount(giantref, 0, true); // Reset limit
				DisableButtTrackTask(giantref);
				
				return false;
			}
			return true;
		});
	}

	void GTSButtCrush_Exit(AnimationEventData& data) {
		auto giant = &data.giant;
		ModGrowthCount(giant, 0, true); // Reset limit
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	///
	///                     T R I G G E R S
	///
	///////////////////////////////////////////////////////////////////////////////////////////////////////////


	void ButtCrushStartEvent(const ManagedInputEvent& data) {
		Actor* player = GetPlayerOrControlled();

		
		if (Runtime::HasPerk(player, "GTSPerkButtCrushAug1")) {
			auto& ButtCrush = ButtCrushController::GetSingleton();

			std::vector<Actor*> preys = ButtCrush.GetButtCrushTargets(player, 1);
			for (auto prey: preys) {
				ButtCrushController::StartButtCrush(player, prey); // attaches actors to AnimObjectB
			} 
			return;
		} else if (CanDoButtCrush(player, true) && !Runtime::HasPerk(player, "GTSPerkButtCrushAug1")) {
			float WasteStamina = 100.0f * GetButtCrushCost(player, false);
			DamageAV(player, ActorValue::kStamina, WasteStamina);
			AnimationManager::StartAnim("ButtCrush_StartFast", player);
		} else if (!CanDoButtCrush(player, false) && !Runtime::HasPerk(player, "GTSPerkButtCrushAug1")) {
			ButtCrushController::ButtCrush_OnCooldownMessage(player);
		}
	}

	void ButtCrushStartEvent_Follower(const ManagedInputEvent& data) {
		Actor* player = PlayerCharacter::GetSingleton();
		ForceFollowerAnimation(player, FollowerAnimType::ButtCrush);
	}

	void QuickButtCrushStartEvent(const ManagedInputEvent& data) {
		auto player = PlayerCharacter::GetSingleton();
		if (CanDoButtCrush(player, true)) {
			AnimationManager::StartAnim("ButtCrush_StartFast", player);
		} else {
			ButtCrushController::ButtCrush_OnCooldownMessage(player);
		}
	}

	void ButtCrushGrowEvent(const ManagedInputEvent& data) {
		Actor* player = GetPlayerOrControlled();
			float GrowthCount = GetGrowthLimit(player);
			bool CanGrow = ButtCrush_IsAbleToGrow(player, GrowthCount);
			if (CanGrow) {
				AnimationManager::StartAnim("ButtCrush_Growth", player);
			} else {
				NotifyWithSound(player, "Your body can't grow any further");
			}
	}

	void ButtCrushAttackEvent(const ManagedInputEvent& data) {
		Actor* player = GetPlayerOrControlled();
		AnimationManager::StartAnim("ButtCrush_Attack", player);
	}
}

namespace GTS
{
	void AnimationButtCrush::RegisterEvents() {
		AnimationManager::RegisterEvent("GTSButtCrush_Exit", "ButtCrush", GTSButtCrush_Exit);
		AnimationManager::RegisterEvent("GTSButtCrush_GrowthStart", "ButtCrush", GTSButtCrush_GrowthStart);
		AnimationManager::RegisterEvent("GTSBEH_ButtCrush_GrowthFinish", "ButtCrush", GTSBEH_ButtCrush_GrowthFinish);
		AnimationManager::RegisterEvent("GTSButtCrush_FallDownStart", "ButtCrush", GTSButtCrush_FallDownStart);
		AnimationManager::RegisterEvent("GTSButtCrush_FallDownImpact", "ButtCrush", GTSButtCrush_FallDownImpact);
		AnimationManager::RegisterEvent("GTSButtCrush_HandImpactR", "ButtCrush", GTSButtCrush_HandImpactR);
		AnimationManager::RegisterEvent("GTSButtCrush_FootstepR", "ButtCrush", GTSButtCrush_FootstepR);
		AnimationManager::RegisterEvent("GTSButtCrush_FootstepL", "ButtCrush", GTSButtCrush_FootstepL);
		AnimationManager::RegisterEvent("GTSButtCrush_MoveBody_MixFrameToLoop", "ButtCrush", GTSButtCrush_MoveBody_MixFrameToLoop);
		AnimationManager::RegisterEvent("GTSButtCrush_MoveBody_Start", "ButtCrush", GTSButtCrush_MoveBody_Start);
		AnimationManager::RegisterEvent("GTSButtCrush_MoveBody_Stop", "ButtCrush", GTSButtCrush_MoveBody_Stop);
		
		InputManager::RegisterInputEvent("ButtCrushStart", ButtCrushStartEvent, ButtCrushCondition_Start);
		InputManager::RegisterInputEvent("ButtCrushStart_Player", ButtCrushStartEvent_Follower, ButtCrushCondition_Follower);
		InputManager::RegisterInputEvent("QuickButtCrushStart", QuickButtCrushStartEvent, ButtCrushCondition_Start);
		InputManager::RegisterInputEvent("ButtCrushGrow", ButtCrushGrowEvent, ButtCrushCondition_Grow);
		InputManager::RegisterInputEvent("ButtCrushAttack", ButtCrushAttackEvent, ButtCrushCondition_Attack);
	}

	void AnimationButtCrush::RegisterTriggers() {
		AnimationManager::RegisterTrigger("ButtCrush_Start", "ButtCrush", "GTSBEH_ButtCrush_Start");
		AnimationManager::RegisterTrigger("ButtCrush_Attack", "ButtCrush", "GTSBEH_ButtCrush_Attack");
		AnimationManager::RegisterTrigger("ButtCrush_Growth", "ButtCrush", "GTSBEH_ButtCrush_Grow");
		AnimationManager::RegisterTrigger("ButtCrush_StartFast", "ButtCrush", "GTSBEH_ButtCrush_StartFast");
	}
}