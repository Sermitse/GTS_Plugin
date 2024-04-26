#include "managers/animation/Controllers/ButtCrushController.hpp"
#include "managers/animation/Utils/CooldownManager.hpp"
#include "managers/animation/Utils/AnimationUtils.hpp"
#include "managers/animation/Utils/CrawlUtils.hpp"
#include "managers/animation/AnimationManager.hpp"
#include "managers/damage/CollisionDamage.hpp"
#include "managers/animation/ButtCrush.hpp"
#include "managers/damage/LaunchActor.hpp"
#include "managers/ai/aifunctions.hpp"
#include "managers/animation/Grab.hpp"
#include "managers/GtsSizeManager.hpp"
#include "managers/InputManager.hpp"
#include "managers/CrushManager.hpp"
#include "managers/explosion.hpp"
#include "managers/audio/footstep.hpp"
#include "managers/highheel.hpp"
#include "utils/actorUtils.hpp"
#include "data/persistent.hpp"
#include "managers/Rumble.hpp"
#include "managers/tremor.hpp"
#include "ActionSettings.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"
#include "data/time.hpp"
#include "node.hpp"

using namespace std;
using namespace SKSE;
using namespace RE;
using namespace Gts;

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

	const std::string_view RNode = "NPC R Foot [Rft ]";
	const std::string_view LNode = "NPC L Foot [Lft ]";

	void StartRumble(std::string_view tag, Actor& actor, float power, float halflife) {
		for (auto& node_name: ALL_RUMBLE_NODES) {
			std::string rumbleName = std::format("ButtCrush_{}{}", tag, node_name);
			Rumbling::Start(rumbleName, &actor, power, halflife, node_name);
		}
	}

	void StopRumble(std::string_view tag, Actor& actor) {
		for (auto& node_name: ALL_RUMBLE_NODES) {
			std::string rumbleName = std::format("ButtCrush_{}{}", tag, node_name);
			Rumbling::Stop(rumbleName, &actor);
		}
	}

	void DisableButtTrackTask(Actor* giant) {
		std::string name = std::format("DisableCamera_{}", giant->formID);
		auto gianthandle = giant->CreateRefHandle();
		auto FrameA = Time::FramesElapsed();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto FrameB = Time::FramesElapsed() - FrameA;
			if (FrameB <= 60.0) {
				return true;
			}
			auto giantref = gianthandle.get().get();
			ManageCamera(giantref, false, CameraTracking::Butt);

			return false;
		});
	}

	void ButtCrush_DoFootImpact(Actor* giant, FootEvent Event, DamageSource Source, std::string_view Node, std::string_view rumble) {
		float perk = GetPerkBonus_Basics(giant);
		float shake = 1.0;
		float dust = 1.0;
		if (HasSMT(giant)) {
			shake = 2.0;
			dust = 1.25;
		}

		float shake_power = Rumble_ButtCrush_FeetImpact * shake * (1.0 + (GetHighHeelsBonusDamage(giant) * 5.0));

		Rumbling::Once(rumble, giant, shake_power, 0.0, Node);
		DoDamageEffect(giant, Damage_ButtCrush_FootImpact, Radius_ButtCrush_FootImpact, 10, 0.25, Event, 1.0, Source);
		DoFootstepSound(giant, 1.0, Event, Node);
		DoDustExplosion(giant, dust, Event, Node);
		DoLaunch(giant, 0.70 * perk, 1.3, Event);
	}

	/////////////////////////////////////////////////////////////////////
	/// EVENTS
	/////////////////////////////////////////////////////////////////////

	

	void GTSButtCrush_MoveBody_MixFrameToLoop(AnimationEventData& data) {
		auto giant = &data.giant;
		ManageCamera(giant, true, CameraTracking::Butt);
		ApplyActionCooldown(giant, CooldownSource::Action_ButtCrush);
	}

	void GTSButtCrush_GrowthStart(AnimationEventData& data) {
		auto giant = &data.giant;

		float scale = get_visual_scale(giant);
		float bonus = 0.24 * (GetGrowthCount(giant) + 1.0);
		float target = std::clamp(bonus/2, 0.02f, 0.80f);
		float gigantism = 1.0 + Ench_Aspect_GetPower(giant);

		ModGrowthCount(giant, 1.0, false);
		SetButtCrushSize(giant, bonus, false);
		SpringGrow_Free(giant, bonus, (0.3 * gigantism) / GetAnimationSlowdown(giant), "ButtCrushGrowth");

		float WasteStamina = 60.0 * GetButtCrushCost(giant);
		DamageAV(giant, ActorValue::kStamina, WasteStamina);

		Runtime::PlaySoundAtNode("growthSound", giant, 1.0, 1.0, "NPC Pelvis [Pelv]");

		StartRumble("BCRumble", data.giant, 0.4, 0.40);
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
			PlayMoanSound(giant, 1.0);
			ApplyActionCooldown(giant, CooldownSource::Emotion_Moan);
			Task_FacialEmotionTask_Moan(giant, 1.8, "ButtCrush_Growth");
		}

		StopRumble("BCRumble", data.giant);
	}

	void GTSButtCrush_FootstepR(AnimationEventData& data) {
		// do footsteps
		ButtCrush_DoFootImpact(&data.giant, FootEvent::Right, DamageSource::CrushedRight, RNode, "FS_R");
		data.HHspeed = 1.0;
	}

	void GTSButtCrush_FootstepL(AnimationEventData& data) {
		// do footsteps
		ButtCrush_DoFootImpact(&data.giant, FootEvent::Left, DamageSource::CrushedLeft, LNode, "FS_L");
		data.HHspeed = 1.0;
	}

	void GTSButtCrush_HandImpactR(AnimationEventData& data) {
		auto giant = &data.giant;
		float scale = get_visual_scale(giant);
		DoCrawlingFunctions(giant, scale, 1.0, Damage_ButtCrush_HandImpact, CrawlEvent::RightHand, "RightHand", 0.5, Radius_ButtCrush_HandImpact, 1.0, DamageSource::HandCrawlRight);
		data.disableHH = false;
		data.HHspeed = 4.0;
	}

	void GTSButtCrush_FallDownStart(AnimationEventData& data) {
		data.stage = 1;
		data.disableHH = true;
		data.HHspeed = 1.0;
	}

	void GTSButtCrush_FallDownImpact(AnimationEventData& data) {
		auto giant = &data.giant;

		float perk = GetPerkBonus_Basics(&data.giant);
		float dust = 1.0;

		if (HasSMT(giant)) {
			dust = 1.25;
		}

		SetButtCrushSize(giant, 0.0, true);

		float damage = GetButtCrushDamage(giant);
		auto ThighL = find_node(giant, "NPC L Thigh [LThg]");
		auto ThighR = find_node(giant, "NPC R Thigh [RThg]");
		auto ButtR = find_node(giant, "NPC R Butt");
		auto ButtL = find_node(giant, "NPC L Butt");

		ApplyThighDamage(giant, true, false, Radius_ThighCrush_ButtCrush_Drop, Damage_ButtCrush_LegDrop * damage, 0.35, 1.0, 14, DamageSource::ThighCrushed);
		ApplyThighDamage(giant, false, false, Radius_ThighCrush_ButtCrush_Drop, Damage_ButtCrush_LegDrop * damage, 0.35, 1.0, 14, DamageSource::ThighCrushed);

		float shake_power = Rumble_ButtCrush_ButtImpact * dust * damage;

		if (ButtR && ButtL) {
			if (ThighL && ThighR) {
				DoDamageAtPoint(giant, Radius_ButtCrush_Impact, Damage_ButtCrush_ButtImpact * damage, ThighL, 4, 0.70, 0.85, DamageSource::Booty);
				DoDamageAtPoint(giant, Radius_ButtCrush_Impact, Damage_ButtCrush_ButtImpact * damage, ThighR, 4, 0.70, 0.85, DamageSource::Booty);
				DoDustExplosion(giant, 1.45 * dust * damage, FootEvent::Butt, "NPC R Butt");
				DoDustExplosion(giant, 1.45 * dust * damage, FootEvent::Butt, "NPC L Butt");
				DoLaunch(&data.giant, 1.30 * perk, 4.20, FootEvent::Butt);
				DoFootstepSound(giant, 1.25, FootEvent::Right, RNode);
				
				
				Rumbling::Once("Butt_L", &data.giant, shake_power, 0.05, "NPC R Butt");
				Rumbling::Once("Butt_R", &data.giant, shake_power, 0.05, "NPC L Butt");
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
		ModGrowthCount(giant, 0, true); // Reset limit
		ApplyActionCooldown(giant, CooldownSource::Action_ButtCrush);
		DisableButtTrackTask(giant);
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


	void ButtCrushStartEvent(const InputEventData& data) {
		Actor* player = GetPlayerOrControlled();

		if (player->formID == 0x14 && IsFirstPerson()) {
			return;
		}
		if (IsGtsBusy(player) || IsChangingSize(player) || !CanPerformAnimation(player, 2)) {
			return;
		}
		auto grabbedActor = Grab::GetHeldActor(player);
		if (grabbedActor && !IsCrawling(player)) { // IF we have someone in hands, allow only when we crawl
			return;
		}
		if (Runtime::HasPerk(player, "ButtCrush_NoEscape")) {
			auto& ButtCrush = ButtCrushController::GetSingleton();

			std::vector<Actor*> preys = ButtCrush.GetButtCrushTargets(player, 1);
			for (auto prey: preys) {
				ButtCrush.StartButtCrush(player, prey); // attaches actors to AnimObjectB
			}
			return;
		} else if (CanDoButtCrush(player, true) && !Runtime::HasPerk(player, "ButtCrush_NoEscape")) {
			float WasteStamina = 100.0 * GetButtCrushCost(player);
			DamageAV(player, ActorValue::kStamina, WasteStamina);
			AnimationManager::StartAnim("ButtCrush_StartFast", player);
		} else if (!CanDoButtCrush(player, false) && !Runtime::HasPerk(player, "ButtCrush_NoEscape")) {
			ButtCrushController::ButtCrush_OnCooldownMessage(player);
		}
	}

	void ButtCrushStartEvent_Follower(const InputEventData& data) {
		Actor* player = PlayerCharacter::GetSingleton();
		ForceFollowerAnimation(player, FollowerAnimType::ButtCrush);
	}

	void QuickButtCrushStartEvent(const InputEventData& data) {
		auto player = PlayerCharacter::GetSingleton();
		if (IsFirstPerson()) {
			return;
		}
		if (GetPlayerOrControlled()->formID != 0x14) {
			if (IsBeingHeld(GetPlayerOrControlled(), player)) {
				return;
			}
		}
		if (!CanPerformAnimation(player, 2) || IsGtsBusy(player)) {
			return;
		}
		auto grabbedActor = Grab::GetHeldActor(player);
		if (grabbedActor && !IsCrawling(player)) { // IF we have someone in hands, allow only when we crawl
			return;
		}
		if (CanDoButtCrush(player, true)) {
			AnimationManager::StartAnim("ButtCrush_StartFast", player);
		} else {
			ButtCrushController::ButtCrush_OnCooldownMessage(player);
		}
	}

	void ButtCrushGrowEvent(const InputEventData& data) {
		Actor* player = GetPlayerOrControlled();
		if (player->formID == 0x14 && IsFirstPerson()) {
			return;
		}
		if (IsButtCrushing(player) && !IsChangingSize(player) && Runtime::HasPerkTeam(player, "ButtCrush_GrowingDisaster")) {
			float GrowthCount = GetGrowthLimit(player);
			bool CanGrow = ButtCrush_IsAbleToGrow(player, GrowthCount);
			if (CanGrow) {
				AnimationManager::StartAnim("ButtCrush_Growth", player);
			} else {
				TiredSound(player, "Your body can't grow any further");
			}
		}
	}

	void ButtCrushAttackEvent(const InputEventData& data) {
		Actor* player = GetPlayerOrControlled();
		if (IsButtCrushing(player)) {
			AnimationManager::StartAnim("ButtCrush_Attack", player);
		}
	}
}

namespace Gts
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

		InputManager::RegisterInputEvent("ButtCrushStart", ButtCrushStartEvent);
		InputManager::RegisterInputEvent("ButtCrushStart_Player", ButtCrushStartEvent_Follower);
		InputManager::RegisterInputEvent("QuickButtCrushStart", QuickButtCrushStartEvent);
		InputManager::RegisterInputEvent("ButtCrushGrow", ButtCrushGrowEvent);
		InputManager::RegisterInputEvent("ButtCrushAttack", ButtCrushAttackEvent);
	}

	void AnimationButtCrush::RegisterTriggers() {
		AnimationManager::RegisterTrigger("ButtCrush_Start", "ButtCrush", "GTSBEH_ButtCrush_Start");
		AnimationManager::RegisterTrigger("ButtCrush_Attack", "ButtCrush", "GTSBEH_ButtCrush_Attack");
		AnimationManager::RegisterTrigger("ButtCrush_Growth", "ButtCrush", "GTSBEH_ButtCrush_Grow");
		AnimationManager::RegisterTrigger("ButtCrush_StartFast", "ButtCrush", "GTSBEH_ButtCrush_StartFast");
	}
}