
#include "Managers/AI/ButtCrush/ButtCrushAI.hpp"
#include "Config/Config.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Grab.hpp"
#include "Managers/Animation/Controllers/ButtCrushController.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Utils/ButtCrushUtils.hpp"

using namespace GTS;

namespace {

	constexpr float MINIMUM_BUTTCRUSH_DISTANCE = 95.0f;
	constexpr float BUTTCRUSH_ANGLE = 70;
	constexpr float PI = std::numbers::pi_v<float>;
	constexpr bool ALLOW_DEAD = false;

	bool CanButtCrush(Actor* a_Performer, Actor* a_Prey) {

		if (a_Performer == a_Prey) {
			return false;
		}

		if ((a_Prey->IsDead() || GetAV(a_Prey, ActorValue::kHealth) < 0.0f) && !ALLOW_DEAD) {
			return false;
		}

		if (!CanPerformAnimationOn(a_Performer, a_Prey, false)) {
			return false;
		}

		const float PredScale = get_visual_scale(a_Performer);
		const float SizeDiff = GetSizeDifference(a_Performer, a_Prey, SizeType::VisualScale, true, false);

		float MINIMUM_BUTTCRUSH_SCALE = Action_ButtCrush;
		constexpr float MINIMUM_DISTANCE = MINIMUM_BUTTCRUSH_DISTANCE;

		if (IsCrawling(a_Performer)) {
			MINIMUM_BUTTCRUSH_SCALE *= 1.5f;
		}

		const float PreyDistance = (a_Performer->GetPosition() - a_Prey->GetPosition()).Length();

		if (PreyDistance <= (MINIMUM_DISTANCE * PredScale) && SizeDiff >= MINIMUM_BUTTCRUSH_SCALE) {
			return true;
		}

		return false;

	}

	void ButtCrushAI_StartLogicTask(Actor* a_Performer) {

		if (!a_Performer) return;

		const std::string TaskName = std::format("ButtCrushAI_{}", a_Performer->formID);

		const auto PerformerHandle = a_Performer->CreateRefHandle();
		const auto BeginTime = Time::WorldTimeElapsed();
		const auto& ActorTransient = Transient::GetActorData(a_Performer);

		TaskManager::Run(TaskName, [=](auto& progressData) {

			if (!PerformerHandle || !ActorTransient) {
				return false;
			}

			const auto FrameB = Time::WorldTimeElapsed() - BeginTime;
			if (FrameB <= 2.0f) {
				return true;
			}

			Actor* ActorRef = PerformerHandle.get().get();

			const bool CanGrow = ButtCrush_IsAbleToGrow(ActorRef, GetGrowthLimit(ActorRef));
			const bool BlockGrowth = IsActionOnCooldown(ActorRef, CooldownSource::Misc_AiGrowth);

			if (IsChangingSize(ActorRef)) { // Growing/shrinking
				ApplyActionCooldown(ActorRef, CooldownSource::Misc_AiGrowth);
			}

			ActorTransient->ActionTimer.UpdateDelta(Config::AI.ButtCrush.fInterval);
			if (BlockGrowth && !ActorTransient->ActionTimer.ShouldRun()) {
				return true;
			}

			if (!IsChangingSize(ActorRef)){
				//If we dont have the perk or for some reason the action needs to be canceled just play the attack anim immediatly
				if (!Runtime::HasPerkTeam(ActorRef, "GTSPerkButtCrushAug2") || !IsButtCrushing(ActorRef)) {
					AnimationManager::StartAnim("ButtCrush_Attack", ActorRef);
				}
				else if (CanGrow && RandomBool(Config::AI.ButtCrush.fGrowProb)) {
					ApplyActionCooldown(ActorRef, CooldownSource::Misc_AiGrowth);
					AnimationManager::StartAnim("ButtCrush_Growth", ActorRef);
				}
				// Can't grow any further or random bool tells us to stop
				else if (!CanGrow || RandomBool(Config::AI.ButtCrush.fCrushProb)) {
					AnimationManager::StartAnim("ButtCrush_Attack", ActorRef);
				}
			}

			if (!IsButtCrushing(ActorRef)) {
				return false; // End the task
			}

			return true;
		});
	}
}

namespace GTS {

	std::vector<Actor*> ButtCrushAI_FilterList(Actor* a_Performer, const std::vector<Actor*>& a_ViablePreyList) {

		if (!a_Performer) {
			return {};
		}

		if (IsGtsBusy(a_Performer) || IsChangingSize(a_Performer)) {
			return {};
		}

		//Don't do action if we're holding an actor
		if (Grab::GetHeldActor(a_Performer)) {
			return {};
		}

		const auto CharController = a_Performer->GetCharController();
		if (!CharController) {
			return {};
		}

		NiPoint3 PredPos = a_Performer->GetPosition();

		auto PreyList = a_ViablePreyList;

		// Sort prey by distance
		ranges::sort(PreyList,[PredPos](const Actor* a_PreyA, const Actor* a_PreyB) -> bool {
			const float DistToA = (a_PreyA->GetPosition() - PredPos).Length();
			const float DistToB = (a_PreyB->GetPosition() - PredPos).Length();
			return DistToA < DistToB;
		});

		// Filter out invalid targets
		std::erase_if(PreyList, [a_Performer](auto idxPrey) {
			return !CanButtCrush(a_Performer, idxPrey);
		});

		// Filter out actors not in front
		const auto ActorAngle = a_Performer->data.angle.z;
		constexpr RE::NiPoint3 FWDVector{ 0.f, 1.f, 0.f };
		const RE::NiPoint3 ActorForward = RotateAngleAxis(FWDVector, -ActorAngle, { 0.f, 0.f, 1.f });

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
		const float ShiftAmount = fabs((PredConeWidth / 2.0f) / tan(BUTTCRUSH_ANGLE / 2.0f));

		const NiPoint3 ConeStart = PredPos - PredDirection * ShiftAmount;
		std::erase_if(PreyList, [ConeStart, PredConeWidth, PredDirection](auto idxPrey) {
			NiPoint3 PreyDirection = idxPrey->GetPosition() - ConeStart;
			if (PreyDirection.Length() <= PredConeWidth * 0.4f) {
				return false;
			}
			PreyDirection = PreyDirection / PreyDirection.Length();
			const float CosineTheta = PredDirection.Dot(PreyDirection);
			return CosineTheta <= cos(BUTTCRUSH_ANGLE * PI / 180.0f);
		});

		return GetMaxActionableTinyCount(a_Performer, PreyList);

	}

    void ButtCrushAI_Start(Actor* A_Performer, Actor* a_Prey) {

		if (RandomBool(Config::AI.ButtCrush.fButtCrushTypeProb) && Runtime::HasPerkTeam(A_Performer, "GTSPerkButtCrushAug1")) {
			ButtCrushController::StartButtCrush(A_Performer, a_Prey, false);
			ButtCrushAI_StartLogicTask(A_Performer);
		}
		else {
			AnimationManager::StartAnim("ButtCrush_StartFast", A_Performer);
		}
    }
}