#include "Managers/Animation/Vore_Crawl.hpp"
#include "Managers/Animation/AnimationManager.hpp"

#include "Managers/Animation/Controllers/VoreController.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CrawlUtils.hpp"
#include "Managers/Rumble.hpp"

using namespace GTS;

namespace {

	constexpr std::string_view RNode = "NPC R Foot [Rft ]";
	constexpr std::string_view LNode = "NPC L Foot [Lft ]";

	const std::vector<std::string_view> RHAND_RUMBLE_NODES = { // used for hand rumble
		"NPC R UpperarmTwist1 [RUt1]",
		"NPC R UpperarmTwist2 [RUt2]",
		"NPC R Forearm [RLar]",
		"NPC R ForearmTwist2 [RLt2]",
		"NPC R ForearmTwist1 [RLt1]",
		"NPC R Hand [RHnd]",
	};

	const std::vector<std::string_view> LHAND_RUMBLE_NODES = { // used for hand rumble
		"NPC L UpperarmTwist1 [LUt1]",
		"NPC L UpperarmTwist2 [LUt2]",
		"NPC L Forearm [LLar]",
		"NPC L ForearmTwist2 [LLt2]",
		"NPC L ForearmTwist1 [LLt1]",
		"NPC L Hand [LHnd]",
	};

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

	void GTSBeh_CrawlVoring(AnimationEventData& data) {
		auto giant = &data.giant;
		auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);
		VoreData.AllowToBeVored(false);
		for (auto& tiny: VoreData.GetVories()) {
			AllowToBeCrushed(tiny, false);
			DisableCollisions(tiny, giant);
			SetBeingHeld(tiny, true);
		}
	}

	void GTSCrawlVore_SmileOn(AnimationEventData& data) {
		AdjustFacialExpression(&data.giant, 2, 1.0f, CharEmotionType::Expression);
		AdjustFacialExpression(&data.giant, 3, 0.8f, CharEmotionType::Phenome);
	}

	void GTSCrawlVore_Grab(AnimationEventData& data) {
		auto giant = &data.giant;
		auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);
		for (auto& tiny: VoreData.GetVories()) {
			if (!Vore_ShouldAttachToRHand(giant, tiny)) {
				VoreData.GrabAll();
			}
			tiny->NotifyAnimationGraph("JumpFall");
			tiny->Attacked(giant);
		}
		if (IsTransferingTiny(giant)) {
			ManageCamera(giant, true, CameraTracking::ObjectA);
		} else {
			ManageCamera(giant, true, CameraTracking::Hand_Right);
		}
	}

	void GTSCrawl_ButtImpact(AnimationEventData& data) {
		auto giant = &data.giant;

		float perk = GetPerkBonus_Basics(&data.giant);
		float dust = 1.0f;
		float smt = 1.0f;

		if (HasSMT(giant)) {
			dust = 1.25f;
			smt = 2.0f;
		}

		auto ThighL = find_node(giant, "NPC L Thigh [LThg]");
		auto ThighR = find_node(giant, "NPC R Thigh [RThg]");
		auto ButtR = find_node(giant, "NPC R Butt");
		auto ButtL = find_node(giant, "NPC L Butt");
		if (ButtR && ButtL) {
			if (ThighL && ThighR) {
				
				ApplyThighDamage(giant, true, false, Radius_ThighCrush_ButtCrush_Drop, Damage_ButtCrush_LegDrop * perk, 0.35f, 1.0f, 14, DamageSource::ThighCrushed);
				ApplyThighDamage(giant, false, false, Radius_ThighCrush_ButtCrush_Drop, Damage_ButtCrush_LegDrop * perk, 0.35f, 1.0f, 14, DamageSource::ThighCrushed);

				DoDamageAtPoint(giant, Radius_Crawl_Vore_ButtImpact, Damage_Crawl_Vore_Butt_Impact * perk, ThighL, 10, 0.70f, 0.95f, DamageSource::Booty);
				DoDamageAtPoint(giant, Radius_Crawl_Vore_ButtImpact, Damage_Crawl_Vore_Butt_Impact * perk, ThighR, 10, 0.70f, 0.95f, DamageSource::Booty);
				DoDustExplosion(giant, 1.8f * dust, FootEvent::Right, "NPC R Butt");
				DoDustExplosion(giant, 1.8f * dust, FootEvent::Left, "NPC L Butt");
				DoFootstepSound(giant, 1.2f, FootEvent::Right, RNode);
				DoFootstepSound(giant, 1.2f, FootEvent::Left, LNode);
				DoLaunch(&data.giant, 2.25f, 5.0f, FootEvent::Butt);

				float shake_power = Rumble_Crawl_KneeDrop/2 * smt;

				Rumbling::Once("Butt_L", &data.giant, shake_power, 0.10f, "NPC R Butt", 0.0f);
				Rumbling::Once("Butt_R", &data.giant, shake_power, 0.10f, "NPC L Butt",  0.0f);
			}
		}
	}

	void GTSCrawlVore_OpenMouth(AnimationEventData& data) {
		auto giant = &data.giant;
		Task_FacialEmotionTask_OpenMouth(giant, 0.5f, "CrawlVoreOpenMouth");

		auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);

		for (auto& tiny: VoreData.GetVories()) {
			VoreController::GetSingleton().ShrinkOverTime(giant, tiny);
		}
	}

	void GTSCrawlVore_CloseMouth(AnimationEventData& data) {
	}
	void GTSCrawlVore_Swallow(AnimationEventData& data) {
		auto giant = &data.giant;
		auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);

		if (!AllowDevourment()) {
			Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundSwallow, giant, 1.0f, "NPC Head [Head]"); // Play sound
		}

		for (auto& tiny: VoreData.GetVories()) {
			AllowToBeCrushed(tiny, true);
			if (tiny->formID == 0x14) {
				PlayerCamera::GetSingleton()->cameraTarget = giant->CreateRefHandle();
			}
			if (AllowDevourment()) {
				CallDevourment(&data.giant, tiny);
				SetBeingHeld(tiny, false);
				VoreData.AllowToBeVored(true);
			} else {
				VoreData.Swallow();
				tiny->SetAlpha(0.0f);
			}
		}
	}

	void GTSCrawlVore_KillAll(AnimationEventData& data) {
		auto giant = &data.giant;
		auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);
		for (auto& tiny: VoreData.GetVories()) {
			if (tiny) {
				AllowToBeCrushed(tiny, true);
				EnableCollisions(tiny);
			}
		}
		VoreData.AllowToBeVored(true);
		VoreData.KillAll();
		VoreData.ReleaseAll();

		ManageCamera(giant, false, CameraTracking::ObjectA);
		ManageCamera(giant, false, CameraTracking::Hand_Right);
	}

	void GTSCrawlVore_SmileOff(AnimationEventData& data) {
		AdjustFacialExpression(&data.giant, 2, 0.0f, CharEmotionType::Expression);
		AdjustFacialExpression(&data.giant, 3, 0.0f, CharEmotionType::Phenome);
	}

	void GTSBEH_CrawlVoreExit(AnimationEventData& data) {} // unused
}


namespace GTS
{
	void Animation_VoreCrawl::RegisterEvents() { 
		AnimationManager::RegisterEvent("GTSBeh_CrawlVoring", "CrawlVore", GTSBeh_CrawlVoring);
		AnimationManager::RegisterEvent("GTSCrawlVore_SmileOn", "CrawlVore", GTSCrawlVore_SmileOn);
		AnimationManager::RegisterEvent("GTSCrawlVore_Grab", "CrawlVore", GTSCrawlVore_Grab);
		AnimationManager::RegisterEvent("GTSCrawl_ButtImpact", "CrawlVore", GTSCrawl_ButtImpact);
		AnimationManager::RegisterEvent("GTSBEH_CrawlVoreExit", "CrawlVore", GTSBEH_CrawlVoreExit);
		AnimationManager::RegisterEvent("GTSCrawlVore_OpenMouth", "CrawlVore", GTSCrawlVore_OpenMouth);
		AnimationManager::RegisterEvent("GTSCrawlVore_CloseMouth", "CrawlVore", GTSCrawlVore_CloseMouth);
		AnimationManager::RegisterEvent("GTSCrawlVore_Swallow", "CrawlVore", GTSCrawlVore_Swallow);
		AnimationManager::RegisterEvent("GTSCrawlVore_KillAll", "CrawlVore", GTSCrawlVore_KillAll);
		AnimationManager::RegisterEvent("GTSCrawlVore_SmileOff", "CrawlVore", GTSCrawlVore_SmileOff);
	}
}
