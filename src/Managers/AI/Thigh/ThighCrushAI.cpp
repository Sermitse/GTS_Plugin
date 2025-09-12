#include "Managers/AI/Thigh/ThighCrushAI.hpp"
#include "Config/Config.hpp"
#include "Managers/HighHeel.hpp"
#include "Managers/Animation/AnimationManager.hpp"

using namespace GTS;

namespace {

	constexpr float MINIMUM_THIGH_DISTANCE = 58.0f;
	constexpr float THIGH_ANGLE = 75;
	constexpr float PI = std::numbers::pi_v<float>;
	constexpr bool ALLOW_DEAD = false;


	bool CanThighCrush(Actor* a_Performer, Actor* a_Prey) {
		if (a_Performer == a_Prey) {
			return false;
		}
		if ((a_Prey->IsDead() || GetAV(a_Prey, ActorValue::kHealth) < 0.0f) && !ALLOW_DEAD) {
			return false;
		}
		if (IsCrawling(a_Performer) || IsTransitioning(a_Performer) || IsBeingHeld(a_Performer, a_Prey)) {
			return false;
		}

		const float PredScale = get_visual_scale(a_Performer);
		const float SizeDiff = GetSizeDifference(a_Performer, a_Prey, SizeType::VisualScale, false, true);
		const float MinimumDistance = MINIMUM_THIGH_DISTANCE + HighHeelManager::GetBaseHHOffset(a_Performer).Length();
		constexpr float MinimumThighCrushScale = Action_AI_ThighCrush;
		const float PreyDistance = (a_Performer->GetPosition() - a_Prey->GetPosition()).Length();

		if (PreyDistance <= MinimumDistance * PredScale) {

			if (SizeDiff > MinimumThighCrushScale) {

				if (CanPerformAnimationOn(a_Performer, a_Prey, false)) {
					return true;
				}
			}
		}
		return false;
	}

	void StartThighCrushTask(Actor* giant) {

		const std::string TaskName = std::format("ThighCrush_{}", giant->formID);

		const ActorHandle GiantHandle = giant->CreateRefHandle();
		const auto& ActorTransient = Transient::GetSingleton().GetData(giant);
		const double StartTime = Time::WorldTimeElapsed();

		TaskManager::Run(TaskName, [=](auto& progressData) {

			if (!Plugin::Live()) return false;

			const auto& ThighSettings = Config::AI.ThighCrush;

			if (!GiantHandle || !ActorTransient || !ThighSettings.bEnableAction) {
				return false;
			}

			Actor* ActorRef = GiantHandle.get().get();

			if (!ActorRef) {
				return false;
			}

			const double FinishTime = Time::WorldTimeElapsed();
			ActorTransient->ActionTimer.UpdateDelta(ThighSettings.fInterval);

			if (FinishTime - StartTime > 0.10) {

				//Are we in a thigh crush anim (idle or acting)
				if (!IsThighCrushing(ActorRef)) {
					return false;
				}

				if (ActorTransient->ActionTimer.ShouldRunFrame()) {

					bool StaminaTooLow = GetAV(ActorRef, ActorValue::kStamina) <= 2.0f;
					DamageAV(ActorRef, ActorValue::kStamina, 0.025f);
					std::vector<Actor*> ValidTargetsInFront = ThighCrushAI_FilterList(ActorRef, find_actors());

					if (StaminaTooLow) {
						AnimationManager::StartAnim("ThighLoopExit", ActorRef);
						return true;
					}
					if (ValidTargetsInFront.empty()) {
						AnimationManager::StartAnim("ThighLoopExit", ActorRef);
						return true;
					}

					//Only Heavy is used
					if (RandomBool(ThighSettings.fProbabilityHeavy)) {
						AnimationManager::StartAnim("ThighLoopAttack", ActorRef);
					}
				}
			}
			return true;
		});
	}
}

namespace GTS {

	//--------------------
	//------- THIGH CRUSH
	//--------------------

	std::vector<Actor*> ThighCrushAI_FilterList(Actor* a_Performer, const std::vector<Actor*>& a_PreyList) {

		if (!a_Performer) {
			return {};
		}

		auto charController = a_Performer->GetCharController();
		if (!charController) {
			return {};
		}

		NiPoint3 PredPos = a_Performer->GetPosition();

		auto PreyList = a_PreyList;

		// Sort prey by distance
		ranges::sort(PreyList,[PredPos](const Actor* a_PreyA, const Actor* a_PreyB) -> bool {
			float DistToA = (a_PreyA->GetPosition() - PredPos).Length();
			float DistToB = (a_PreyB->GetPosition() - PredPos).Length();
			return DistToA < DistToB;
		});

		// Filter out invalid targets
		std::erase_if(PreyList, [a_Performer](auto idxPrey) {
			return !CanThighCrush(a_Performer, idxPrey);
		});

		// Filter out actors not in front
		auto ActorAngle = a_Performer->data.angle.z;
		constexpr NiPoint3 FWDVector { 0.f, 1.f, 0.f };
		NiPoint3 ActorForward = RotateAngleAxis(FWDVector, -ActorAngle, { 0.f, 0.f, 1.f });

		NiPoint3 PredDirection = ActorForward;
		PredDirection = PredDirection / PredDirection.Length();
		std::erase_if(PreyList, [PredPos, PredDirection](auto idxPrey) {
			NiPoint3 PreyDir = idxPrey->GetPosition() - PredPos;
			if (PreyDir.Length() <= 1e-4) {
				return false;
			}
			PreyDir = PreyDir / PreyDir.Length();
			float CosineTheta = PredDirection.Dot(PreyDir);
			return CosineTheta <= 0; // 180 degress
		});

		// Filter out actors not in a truncated cone
		// \      x   /
		//  \  x     /
		//   \______/  <- Truncated cone
		//   | pred |  <- Based on width of pred
		//   |______|

		float PredConeWidth = 70 * get_visual_scale(a_Performer);
		float ShiftAmount = fabs((PredConeWidth / 2.0f) / tan(THIGH_ANGLE / 2.0f));

		NiPoint3 ConeStart = PredPos - PredDirection * ShiftAmount;
		std::erase_if(PreyList, [ConeStart, PredConeWidth, PredDirection](auto idxPrey) {
			NiPoint3 PreyDir = idxPrey->GetPosition() - ConeStart;
			if (PreyDir.Length() <= PredConeWidth * 0.4f) {
				return false;
			}
			PreyDir = PreyDir / PreyDir.Length();
			float CosineTheta = PredDirection.Dot(PreyDir);
			return CosineTheta <= cos(THIGH_ANGLE * PI / 180.0f);
		});

		return PreyList;
	}

	void ThighCrushAI_Start(Actor* a_Performer) {

		if (IsThighCrushing(a_Performer)) {
			return;
		}

		AnimationManager::StartAnim("ThighLoopEnter", a_Performer);

		StartThighCrushTask(a_Performer);
	}
}
