#include "Managers/AI/Hug/HugAI.hpp"

#include "Config/Config.hpp"

#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/HugShrink.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"

using namespace GTS;

namespace {

	constexpr float MINIMUM_HUG_DISTANCE = 110.0f;
	constexpr float GRAB_ANGLE = 70.0f;
	constexpr float PI = std::numbers::pi_v<float>;

	void RecordSneakingState(Actor* a_Performer, Actor* a_Prey) {
		bool Crawling = AnimationVars::Crawl::IsCrawling(a_Performer);
		bool Sneaking = a_Performer->IsSneaking();

		AnimationVars::Tiny::SetIsBeingCrawlHugged(a_Prey, Crawling);
		AnimationVars::Tiny::SetIsBeingSneakHugged(a_Prey, Sneaking);
	}

	bool ActorStateCheck(Actor* a_Actor) {
		bool Jumping = IsJumping(a_Actor);
		bool Ragdolled = IsRagdolled(a_Actor);
		bool Busy = IsGtsBusy(a_Actor);
		return Jumping || Ragdolled || Busy;
	}

	bool CanHug(Actor* a_Performer, Actor* a_Prey) {

		if (a_Performer == a_Prey) {
			return false;
		}

		if (a_Prey->IsDead() || GetAV(a_Prey, ActorValue::kHealth) < 0.0f) {
			return false;
		}

		if (IsTransitioning(a_Performer) || IsBeingHeld(a_Performer, a_Prey)) {
			return false;
		}

		if (ActorStateCheck(a_Performer) || ActorStateCheck(a_Prey)) {
			return false;
		}

		const float PredatorScale = get_visual_scale(a_Performer);
		// No need to check for BB scale in this case

		float SizeDifference = get_scale_difference(a_Performer, a_Prey, SizeType::VisualScale, false, true);


		float MinDist = MINIMUM_HUG_DISTANCE;
		constexpr float MinScale = Action_Hug;

		if (a_Performer->IsSneaking()) {
			if (AnimationVars::Crawl::IsCrawling(a_Performer)) {
				MinDist *= 2.35f;
			}
			else {
				MinDist *= 1.6f;
			}
		}

		const float PreyDistance = (a_Performer->GetPosition() - a_Prey->GetPosition()).Length();

		if (PreyDistance <= MinDist * PredatorScale) {

			if (SizeDifference > MinScale) {

				if (a_Prey->formID != 0x14 && !CanPerformAnimationOn(a_Performer, a_Prey, true)) {
					return false;
				}

				if (!IsHuman(a_Prey)) { // Allow hugs with humanoids only
					return false;
				}

				return SizeDifference <= GetHugShrinkThreshold(a_Performer);
			}
		}
		return false;
	}

	//--------------
	// HUG TASK
	//--------------

	bool HugAI_CanHugCrush(Actor* a_Performer, Actor* a_Prey) {

		const float HealthPercentage = GetHealthPercentage(a_Prey);
		const float StaminaPercentage = GetStaminaPercentage(a_Performer);
		const float HPCrushThreshold = GetHugCrushThreshold(a_Performer, a_Prey, true);
		const bool HasLowHP = HealthPercentage <= HPCrushThreshold;
		const bool StaminaCheck = Runtime::HasPerkTeam(a_Performer, Runtime::PERK.GTSPerkHugMightyCuddles) && StaminaPercentage >= 0.75f;
		const auto& Settings = Config::AI.Hugs;
		const bool Teammate = IsTeammate(a_Prey) || a_Prey->formID == 0x14;
		const bool Hostile = IsHostile(a_Performer, a_Prey) || IsHostile(a_Prey, a_Performer);
		const bool CanStartCrush = Config::AI.Hugs.fKillProb > 0.01f;

		const bool Killable =
			(Settings.bKillFollowersOrPlayer && Teammate) || //Teammate/Player Check
			((Settings.bKillFriendlies || Hostile) && !Teammate); //If Kill Friendly is enabled or Tiny is hostile And Not a teammate Allow

		return (HasLowHP || StaminaCheck) && Killable && CanStartCrush;
	}


	bool HugAI_CanShrink(Actor* a_Performer, Actor* a_Prey) {
		const float SizeDiff = get_scale_difference(a_Performer, a_Prey, SizeType::TargetScale, false, true);
		const bool TooSmall = SizeDiff >= GetHugShrinkThreshold(a_Performer);
		const bool CanStartShrink = Config::AI.Hugs.fShrinkProb > 0.01f;

		return !TooSmall && CanStartShrink;
	}

	bool HugAI_CanHeal(Actor* a_Performer, Actor* a_Prey) {
		const bool Teammate = IsTeammate(a_Performer) && (IsTeammate(a_Prey) || a_Prey->formID == 0x14);
		const bool HasPerk = Runtime::HasPerkTeam(a_Performer, Runtime::PERK.GTSPerkHugsLovingEmbrace);
		const bool Hostile = IsHostile(a_Performer, a_Prey) || IsHostile(a_Prey, a_Performer);
		const bool CanStartHeal = Config::AI.Hugs.fHealProb > 0.01f;
		
		return HasPerk && !Hostile && Teammate && CanStartHeal;
	}

	bool HugAI_ShouldStop(Actor* a_Performer, Actor* a_Prey) {
		const bool Teammate = IsTeammate(a_Performer) && (IsTeammate(a_Prey) || a_Prey->formID == 0x14);
		const bool CanCrush = HugAI_CanHugCrush(a_Performer, a_Prey);
		const bool CanHeal = HugAI_CanHeal(a_Performer, a_Prey);
		const bool CanShrink = HugAI_CanShrink(a_Performer, a_Prey) || (!Config::AI.Hugs.bStopIfCantShrink && Teammate);

		return !(CanCrush || CanHeal || CanShrink);
	}

	void HugAI_StartLogicTask(Actor* a_Performer, Actor* a_Prey) {


		const std::string TaskName = std::format("HugAI_{}", a_Performer->formID);
		const ActorHandle PerformerHandle = a_Performer->CreateRefHandle();
		const ActorHandle PreyHandle = a_Prey->CreateRefHandle();

		TaskManager::Run(TaskName, [=](auto& progressData) {

			if (!State::Live()) return false;

			const auto& Settings = Config::AI.Hugs;

			if (!PerformerHandle || !PreyHandle) {
				return false;
			}

			Actor* PerformerActor = PerformerHandle.get().get();
			Actor* PreyActor = PreyHandle.get().get();

			if (!PerformerActor || !PreyActor) {
				return false;
			}

			const auto& TransientData = Transient::GetActorData(PerformerActor);
			if (!TransientData) {
				return false;
			}
			TransientData->ActionTimer.UpdateDelta(Config::AI.Hugs.fInterval);

			const bool IsDead = PreyActor->IsDead() || PerformerActor->IsDead();
			const bool IsBusy = IsHugCrushing(PerformerActor) || IsHugHealing(PerformerActor);
			const bool GentleAnim = IsTeammate(PreyActor) || PreyActor->formID == 0x14;

			if (!HugShrink::GetHuggiesActor(PerformerActor) || IsRagdolled(PerformerActor)) {
				if (!GentleAnim) {
					PushActorAway(PerformerActor, PreyActor, 1.0f);
				}
				return false;
			}

			//Should we stop? Means we can't do any other actions on the Tiny
			const bool ShouldStop = HugAI_ShouldStop(PerformerActor, PreyActor);

			if (TransientData->ActionTimer.ShouldRun() && !IsBusy && !IsDead && !ShouldStop) {

				UpdateFriendlyHugs(PerformerActor, PreyActor, !GentleAnim);
				const bool Teammate = (IsTeammate(PerformerActor) && (IsTeammate(PreyActor) || PreyActor->formID == 0x14));
				//Reduce if folllower or player

				const float ShrinkProbability = Teammate ? Settings.fFriendlyShrinkProb : Settings.fShrinkProb;
				const int SelectedAction = RandomIntWeighted({
					HugAI_CanHeal(PerformerActor,PreyActor) ? static_cast<int>(Settings.fHealProb) : 0,
					HugAI_CanShrink(PerformerActor,PreyActor) ? static_cast<int>(ShrinkProbability) : 0,
					HugAI_CanHugCrush(PerformerActor,PreyActor) ? static_cast<int>(Settings.fKillProb) : 0,
					100 //Base Chance to do nothing
				});


				switch (SelectedAction) {
					case 0:{ //HEAL
						StartHealingAnimation(PerformerActor, PreyActor);
						break;
					}

					case 1:{ //SHRINK
						AnimationManager::StartAnim("Huggies_Shrink", PerformerActor);
						AnimationManager::StartAnim("Huggies_Shrink_Victim", PreyActor);
						break;
					}

					case 2:{ //CRUSH
						AnimationManager::StartAnim("Huggies_HugCrush", PerformerActor);
						AnimationManager::StartAnim("Huggies_HugCrush_Victim", PreyActor);
						break;
					}

					default: {}
				}

			}

			if (ShouldStop) {
				UpdateFriendlyHugs(PerformerActor, PreyActor, !GentleAnim);
				AbortHugAnimation(PerformerActor, PreyActor);
			}

			if (IsDead) {
				return false;
			}

			return true;
		});
	}

}

namespace GTS {

	std::vector<Actor*> HugAI_FilterList(Actor* a_Performer, const std::vector<Actor*>& a_PotentialPrey) {
		// Get vore target for actor

		if (!a_Performer) {
			return {};
		}

		if (IsGtsBusy(a_Performer)) {
			return {};
		}

		if (GetStaminaPercentage(a_Performer) < 0.25f) {
			return {};
		}

		if (!CanDoPaired(a_Performer) && !IsSynced(a_Performer) && !IsTransferingTiny(a_Performer)) {
			return {};
		}

		const auto CharacterController = a_Performer->GetCharController();
		if (!CharacterController) {
			return {};
		}

		const NiPoint3 PredPos = a_Performer->GetPosition();

		auto PreyList = a_PotentialPrey;

		// Sort prey by distance
		ranges::sort(PreyList,[PredPos](const Actor* a_PreyA, const Actor* a_PreyB) -> bool {
			float DistToA = (a_PreyA->GetPosition() - PredPos).Length();
			float DistToB = (a_PreyB->GetPosition() - PredPos).Length();
			return DistToA < DistToB;
		});

		// Filter out invalid targets
		std::erase_if(PreyList, [a_Performer](auto idxPrey) {
			return !CanHug(a_Performer, idxPrey);
		});

		// Filter out actors not in front
		const auto ActorAngle = a_Performer->data.angle.z;
		constexpr NiPoint3 FWDVector{ 0.f, 1.f, 0.f };
		const NiPoint3 ActorForward = RotateAngleAxis(FWDVector, -ActorAngle, { 0.f, 0.f, 1.f });

		NiPoint3 PredDirection = ActorForward;
		PredDirection = PredDirection / PredDirection.Length();
		std::erase_if(PreyList, [PredPos, PredDirection](auto idxPrey) {
			NiPoint3 PreyDirection = idxPrey->GetPosition() - PredPos;
			if (PreyDirection.Length() <= 1e-4) {
				return false;
			}
			PreyDirection = PreyDirection / PreyDirection.Length();
			const float CosineTheta = PredDirection.Dot(PreyDirection);
			return CosineTheta <= 0; // 180 degress
		});

		// Filter out actors not in a truncated cone
		// \      x   /
		//  \  x     /
		//   \______/  <- Truncated cone
		//   | pred |  <- Based on width of pred
		//   |______|

		const float PredConeWidth = 70 * get_visual_scale(a_Performer);
		const float ShiftAmount = fabs((PredConeWidth / 2.0f) / tan(GRAB_ANGLE / 2.0f));

		NiPoint3 ConeStart = PredPos - PredDirection * ShiftAmount;
		std::erase_if(PreyList, [ConeStart, PredConeWidth, PredDirection](auto idxPrey) {
			NiPoint3 PreyDirection = idxPrey->GetPosition() - ConeStart;
			if (PreyDirection.Length() <= PredConeWidth * 0.4f) {
				return false;
			}
			PreyDirection = PreyDirection / PreyDirection.Length();
			const float CosineTheta = PredDirection.Dot(PreyDirection);
			return CosineTheta <= cos(GRAB_ANGLE * PI / 180.0f);
		});


		return PreyList;
	}

	void HugAI_Start(Actor* a_Performer, Actor* a_Prey) {

		RecordSneakingState(a_Performer, a_Prey); // Needed to determine which hugs to play: sneak or crawl ones (when sneaking)
		HugShrink::HugActor(a_Performer, a_Prey);

		AnimationManager::StartAnim("Huggies_Try", a_Performer);

		if (a_Performer->IsSneaking()) {
			AnimationManager::StartAnim("Huggies_Try_Victim_S", a_Prey); // GTSBEH_HugAbsorbStart_Sneak_V
		}
		else {
			AnimationManager::StartAnim("Huggies_Try_Victim", a_Prey); //   GTSBEH_HugAbsorbStart_V
		}

		DisarmActor(a_Prey, false);
		HugAI_StartLogicTask(a_Performer, a_Prey);
	}
}

