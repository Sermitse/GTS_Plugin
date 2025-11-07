#include "Managers/AI/Vore/VoreAI.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Controllers/VoreController.hpp"

namespace {

	constexpr float BASE_CONE_WIDTH = 70.0f;
	constexpr float VORE_ANGLE = 75.0f;
	constexpr float PI = std::numbers::pi_v<float>;

	constexpr bool ALLOW_DEAD = false;

	constexpr float MINIMUM_VORE_SCALE = GTS::Action_Vore;
	constexpr float MINIMUM_DISTANCE = 95.0f;
}


namespace GTS {

	static bool VoreAI_CanVore(Actor* a_Pred, Actor* a_Prey) {

		if (a_Pred == a_Prey) {
			return false;
		}

		if ((a_Prey->IsDead() || GetAV(a_Prey, ActorValue::kHealth) < 0.0f) && !ALLOW_DEAD) {
			return false;
		}

		if (IsBeingHeld(a_Pred, a_Prey)) {
			return false;
		}

		const auto Transient = Transient::GetActorData(a_Prey);
		if (Transient) {
			if (Transient->CanBeVored == false) {
				return false;
			}
		}

		const float PredScale = get_visual_scale(a_Pred);
		const float SizeDiff = GetSizeDifference(a_Pred, a_Prey, SizeType::VisualScale, true, false);
		const float PreyDistance = (a_Pred->GetPosition() - a_Prey->GetPosition()).Length();

		if (IsInsect(a_Prey, true) || IsBlacklisted(a_Prey)) {
			return false;
		}

		if (!IsLiving(a_Prey)) {
			return false;
		}

		if (!CanPerformAnimationOn(a_Pred,a_Prey,false)) {
			return false;
		}

		if (PreyDistance <= (MINIMUM_DISTANCE * PredScale) && SizeDiff > MINIMUM_VORE_SCALE) {
			return true;
		}

		return false;
	}

	vector<Actor*> VoreAI_FilterList(Actor* a_Pred, const vector<Actor*>& a_PotentialPrey) {

		// Get vore target for actor
		if (!a_Pred) {
			return {};
		}

		auto CharController = a_Pred->GetCharController();
		if (!CharController) {
			return {};
		}

		NiPoint3 PredatorPosition = a_Pred->GetPosition();

		auto PreyList = a_PotentialPrey;

		// Sort prey by distance
		ranges::sort(PreyList, [PredatorPosition](const Actor* preyA, const Actor* preyB) -> bool {
			float distanceToA = (preyA->GetPosition() - PredatorPosition).Length();
			float distanceToB = (preyB->GetPosition() - PredatorPosition).Length();
			return distanceToA < distanceToB;
		});

		// Filter out invalid targets
		std::erase_if(PreyList, [a_Pred](auto idxPrey) {
			return !VoreAI_CanVore(a_Pred, idxPrey);
		});

		// Filter out actors not in front
		const float ActorAngle = a_Pred->data.angle.z;
		constexpr NiPoint3 forwardVector{ 0.0f, 1.0f, 0.0f };
		const NiPoint3 ActorForward = RotateAngleAxis(forwardVector, -ActorAngle, { 0.0f, 0.0f, 1.0f });

		NiPoint3 PredatorDirection = ActorForward;
		PredatorDirection = PredatorDirection / PredatorDirection.Length();

		std::erase_if(PreyList, [PredatorPosition, PredatorDirection](auto idxPrey) {
			NiPoint3 PreyDirection = idxPrey->GetPosition() - PredatorPosition;

			if (PreyDirection.Length() <= 1e-4) {
				return false;
			}

			PreyDirection = PreyDirection / PreyDirection.Length();
			float CosineTheta = PredatorDirection.Dot(PreyDirection);
			return CosineTheta <= 0; // 180 degress
		});

		// Filter out actors not in a truncated cone
		// \      x   /
		//  \  x     /
		//   \______/  <- Truncated cone
		//   | pred |  <- Based on width of pred
		//   |______|

		const float PredConeWidth = BASE_CONE_WIDTH * get_visual_scale(a_Pred);
		const float ShiftAmmount = fabs((PredConeWidth / 2.0f) / tan(VORE_ANGLE / 2.0f));
		const NiPoint3 ConeStart = PredatorPosition - PredatorDirection * ShiftAmmount;

		std::erase_if(PreyList, [ConeStart, PredConeWidth, PredatorDirection](auto idxPrey) {
			NiPoint3 PreyDirection = idxPrey->GetPosition() - ConeStart;

			if (PreyDirection.Length() <= PredConeWidth * 0.4f) {
				return false;
			}

			PreyDirection = PreyDirection / PreyDirection.Length();
			const float CosineTheta = PredatorDirection.Dot(PreyDirection);
			return CosineTheta <= cos(VORE_ANGLE * PI / 180.0f);
		});

		return GetMaxActionableTinyCount(a_Pred, PreyList);
	}

	void VoreAI_StartVore(Actor* a_Predator, const vector<Actor*>& a_PotentialPrey) {

		auto& VoreData = VoreController::GetSingleton().GetVoreData(a_Predator);
		for (auto Prey : a_PotentialPrey) {
			VoreData.AddTiny(Prey);
		}

		DamageAV(a_Predator, ActorValue::kStamina, 30 * a_PotentialPrey.size());
		AnimationManager::StartAnim("StartVore", a_Predator);
	}
}



