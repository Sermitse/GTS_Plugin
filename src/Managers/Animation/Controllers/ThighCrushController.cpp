#include "Managers/Animation/Controllers/ThighCrushController.hpp"
#include "Managers/GTSSizeManager.hpp"
#include "Managers/HighHeel.hpp"

namespace {

	constexpr float MINIMUM_THIGH_DISTANCE = 58.0f;
	constexpr float THIGH_ANGLE = 75;
	constexpr float PI = std::numbers::pi_v<float>;

}

namespace GTS {

	std::vector<Actor*> ThighCrushController::GetThighTargetsInFront(Actor* pred, std::size_t numberOfPrey) {
		// Get vore target for actor
		auto& sizemanager = SizeManager::GetSingleton();
		if (!pred) {
			return {};
		}
		auto charController = pred->GetCharController();
		if (!charController) {
			return {};
		}

		NiPoint3 predPos = pred->GetPosition();

		auto preys = find_actors();

		// Sort prey by distance
		sort(preys.begin(), preys.end(),
		     [predPos](const Actor* preyA, const Actor* preyB) -> bool
		{
			float distanceToA = (preyA->GetPosition() - predPos).Length();
			float distanceToB = (preyB->GetPosition() - predPos).Length();
			return distanceToA < distanceToB;
		});

		// Filter out invalid targets
		preys.erase(std::remove_if(preys.begin(), preys.end(),[pred, this](auto prey)
		{
			return !this->CanThighCrush(pred, prey);
		}), preys.end());

		// Filter out actors not in front
		auto actorAngle = pred->data.angle.z;
		RE::NiPoint3 forwardVector{ 0.f, 1.f, 0.f };
		RE::NiPoint3 actorForward = RotateAngleAxis(forwardVector, -actorAngle, { 0.f, 0.f, 1.f });

		NiPoint3 predDir = actorForward;
		predDir = predDir / predDir.Length();
		preys.erase(std::remove_if(preys.begin(), preys.end(),[predPos, predDir](auto prey)
		{
			NiPoint3 preyDir = prey->GetPosition() - predPos;
			if (preyDir.Length() <= 1e-4) {
				return false;
			}
			preyDir = preyDir / preyDir.Length();
			float cosTheta = predDir.Dot(preyDir);
			return cosTheta <= 0; // 180 degress
		}), preys.end());

		// Filter out actors not in a truncated cone
		// \      x   /
		//  \  x     /
		//   \______/  <- Truncated cone
		//   | pred |  <- Based on width of pred
		//   |______|
		float predWidth = 70 * get_visual_scale(pred);
		float shiftAmount = fabs((predWidth / 2.0f) / tan(THIGH_ANGLE/2.0f));

		NiPoint3 coneStart = predPos - predDir * shiftAmount;
		preys.erase(std::remove_if(preys.begin(), preys.end(),[coneStart, predWidth, predDir](auto prey)
		{
			NiPoint3 preyDir = prey->GetPosition() - coneStart;
			if (preyDir.Length() <= predWidth*0.4f) {
				return false;
			}
			preyDir = preyDir / preyDir.Length();
			float cosTheta = predDir.Dot(preyDir);
			return cosTheta <= cos(THIGH_ANGLE*PI/180.0f);
		}), preys.end());

		// Reduce vector size
		if (preys.size() > numberOfPrey) {
			preys.resize(numberOfPrey);
		}

		return preys;
	}

	bool ThighCrushController::CanThighCrush(Actor* pred, Actor* prey) {
		if (pred == prey) {
			return false;
		}

		if (prey->IsDead()) {
			return false;
		}

		if (IsCrawling(pred) || IsTransitioning(pred) || IsBeingHeld(pred, prey)) {
			return false;
		}

		if (pred->AsActorState()->GetSitSleepState() == SIT_SLEEP_STATE::kIsSitting) { // disallow doing it when using furniture
			return false;	
		}

		float pred_scale = get_visual_scale(pred);
		// No need to check for BB scale in this case

		float sizedifference = get_scale_difference(pred, prey, SizeType::VisualScale, false, true);
		
		float MINIMUM_DISTANCE = MINIMUM_THIGH_DISTANCE + HighHeelManager::GetBaseHHOffset(pred).Length();
		float MINIMUM_THIGHCRUSH_SCALE = Action_AI_ThighCrush;

		if (pred->formID == 0x14) {
			MINIMUM_THIGHCRUSH_SCALE = 1.5f; // Used to freeze tinies, Player Only
		}

		float prey_distance = (pred->GetPosition() - prey->GetPosition()).Length();
		
		if (prey_distance <= (MINIMUM_DISTANCE * pred_scale)) {
			if (sizedifference > MINIMUM_THIGHCRUSH_SCALE) {
				if ((prey->formID != 0x14 && !CanPerformAnimationOn(pred, prey, false))) {
					return false;
				}
				return true;
			} else {
				return false;
			}
			return false;
		}
		return false;
	}
}