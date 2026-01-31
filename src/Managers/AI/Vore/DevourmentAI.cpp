#include "Managers/AI/Vore/DevourmentAI.hpp"


namespace {

	constexpr float BASE_CONE_WIDTH = 70.0f;
	constexpr float VORE_ANGLE = 75.0f;
	constexpr float PI = std::numbers::pi_v<float>;
	constexpr float MINIMUM_DISTANCE = 60.0f;
}


namespace GTS {

	static bool VoreAI_CanVore(Actor* a_Pred, Actor* a_Prey) {

		if (a_Pred == a_Prey) {
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
		const float PreyDistance = (a_Pred->GetPosition() - a_Prey->GetPosition()).Length();


		if (PreyDistance <= MINIMUM_DISTANCE * PredScale) {
			return true;
		}

		return false;
	}

	std::vector<Actor*> DevourmentAI_FilterList(Actor* a_Pred, const std::vector<Actor*>& a_PotentialPrey) {

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
		std::ranges::sort(PreyList, [PredatorPosition](const Actor* preyA, const Actor* preyB) -> bool {
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

		return PreyList;
	}

	void DevourmentAI_Start(Actor* a_Predator, const std::vector<Actor*>& a_PotentialPrey) {
		DamageAV(a_Predator, ActorValue::kStamina, 30.0f);
		CallDevourment(a_Predator, a_PotentialPrey.front());
	}
}



