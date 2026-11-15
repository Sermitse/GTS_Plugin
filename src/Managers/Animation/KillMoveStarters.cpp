#include "Managers/Animation/KillMoveStarters.hpp"
#include "Managers/Animation/AnimationManager.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Damage/LaunchObject.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Utils/Actions/AutoAim/AutoAimUtils.hpp"

#include "Managers/Size_Killmoves/SizeKillMove.hpp"
#include "Managers/Size_Killmoves/KillMoveHelper.hpp"

#include "Managers/Size_Killmoves/KillMoveParamObtainer.hpp"

using namespace GTS;

// This file manages kill-move trigger logic for some actions. Note that all standings stomps have these enabled by default, and they're not managed here.

namespace SharedLogic {
	AimOutcome GetLastAimOutcome(Actor* giant) {
		if (auto tranData = Transient::GetActorData(giant)) {
			return tranData->lastAimOutcome;
		}
		return AimOutcome();
	}
	void ClearLastAimData(Actor* giant) {
		if (auto tranData = Transient::GetActorData(giant)) {
			tranData->lastAimOutcome = AimOutcome();
		}
	}
	void LegKillMove(Actor* giant, bool strong) {
		auto LastAim = GetLastAimOutcome(giant);
		LastAim.isSideBlend = true; LastAim.trackEnemy = true;
		
		if (LastAim.victim) {
			AutoAimTask_TrackTinyTask(giant, LastAim.victim, LastAim);
			TryKillMove(giant, LastAim, GetStandingKillMoveParams(LastAim, strong, false));
		}
		
		ClearLastAimData(giant);
	}
	void CrawlBreastKillMove(Actor* giant) {
		auto LastAim = GetLastAimOutcome(giant);
		if (LastAim.victim) {
			if (AnimationVars::Stomp::GetStompBlend_X(giant) > 0.01f && LastAim.inPercent_Side > 0.3f) { 
				// We want to trigger it only when actor is in front and is in boobs range
				TryKillMove(giant, LastAim, GetCrawlKillMoveParams(LastAim));
			}
		}

		ClearLastAimData(giant);
	}
	void CrawlButtKillMove(Actor* giant, bool strong) {
		auto LastAim = GetLastAimOutcome(giant);

		if (LastAim.victim) {
			TryKillMove(giant, LastAim, GetSneakKillMoveParams(LastAim, strong));
		}
		ClearLastAimData(giant);
	}
}

namespace {
	// ======================================================================================
	//  Animation Triggers
	// ======================================================================================

	void GTS_KillMoveCheck_Crawl_Boobs(AnimationEventData& data) {
		SharedLogic::CrawlBreastKillMove(&data.giant);
	}

	void GTS_KillMoveCheck_Crawl_Butt(AnimationEventData& data) {
		SharedLogic::CrawlButtKillMove(&data. giant, true);
	}
	void GTS_KillMoveCheck_Sneak_Butt(AnimationEventData& data) {
		SharedLogic::CrawlButtKillMove(&data. giant, true);
	}
	void GTS_KillMoveCheck_Crawl_Leg(AnimationEventData& data) {
		DrainStamina(&data.giant,"StaminaDrain_HandSlam", Runtime::PERK.GTSPerkDestructionBasics, true, 2.825f);
		SharedLogic::LegKillMove(&data.giant, false);
	}
	void GTS_KillMoveCheck_Crawl_Leg_Strong(AnimationEventData& data) {
		DrainStamina(&data.giant,"StaminaDrain_HandSlam", Runtime::PERK.GTSPerkDestructionBasics, true, 2.6f); // Long anim, needs weaker stamina drain over time
		SharedLogic::LegKillMove(&data.giant, true);
	}
}

namespace GTS
{
	void AnimationKillMoveStarters::RegisterEvents() {
		
		AnimationManager::RegisterEvent("GTS_KillMoveCheck_Crawl_Boobs", "KillMoves", GTS_KillMoveCheck_Crawl_Boobs);
		AnimationManager::RegisterEvent("GTS_KillMoveCheck_Crawl_Butt", "KillMoves", GTS_KillMoveCheck_Crawl_Butt);
		AnimationManager::RegisterEvent("GTS_KillMoveCheck_Sneak_Butt", "KillMoves", GTS_KillMoveCheck_Sneak_Butt);
		AnimationManager::RegisterEvent("GTS_KillMoveCheck_Crawl_Leg", "KillMoves", GTS_KillMoveCheck_Crawl_Leg);
		AnimationManager::RegisterEvent("GTS_KillMoveCheck_Crawl_Leg_Strong", "KillMoves", GTS_KillMoveCheck_Crawl_Leg_Strong);
	}
	void AnimationKillMoveStarters::HandleStandingKillMove(Actor* giant, AimOutcome outcome, bool strong_Attack, bool trample) {
		if (giant && outcome.hit) {
            auto params = GetStandingKillMoveParams(outcome, strong_Attack, trample);
            TryKillMove(giant, outcome, params); // All standing stomps can trigger killmoves, no need for custom events
        }
	}
}