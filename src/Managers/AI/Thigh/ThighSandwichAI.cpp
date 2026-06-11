#include "Managers/AI/Thigh/ThighSandwichAI.hpp"
#include "Config/Config.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Controllers/ThighSandwichController.hpp"

using namespace GTS;

namespace {

	constexpr float MINIMUM_SANDWICH_DISTANCE = 70.0f;
	constexpr float SANDWICH_ANGLE = 60;
	constexpr float PI = std::numbers::pi_v<float>;
	constexpr bool ALLOW_DEAD = false;

	bool CanSandwich(Actor* a_Performer, Actor* a_Prey) {

		if (a_Performer == a_Prey) {
			return false;
		}

		if ((a_Prey->IsDead() || GetAV(a_Prey, ActorValue::kHealth) < 0.0f) && !ALLOW_DEAD) {
			return false;
		}

		if (IsBeingHeld(a_Performer, a_Prey)) {
			return false;
		}

		if (!CanPerformActionOn(a_Performer, a_Prey, false)) {
			return false;
		}

		const float PredScale = get_visual_scale(a_Performer);
		const float SizeDiff = get_scale_difference(a_Performer, a_Prey, SizeType::VisualScale, true, false);
		constexpr float MinimumScale = Action_Sandwich;
		constexpr float MinimumDistance = MINIMUM_SANDWICH_DISTANCE;

		float PreyDistance = (a_Performer->GetPosition() - a_Prey->GetPosition()).Length();

		if (PreyDistance <= (MinimumDistance * PredScale) && SizeDiff > MinimumScale) {
			return true;
		}

		return false;
	}

	void PerformAnimations(RE::Actor* a_performer, std::string_view owner_anim, std::string_view receiver_anim = "") {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(a_performer);
		AnimationManager::StartAnim(owner_anim, a_performer);

		for (Actor* tiny : sandwichdata.GetActors()) {
			if (tiny && receiver_anim.size() > 1) {
				AnimationManager::StartAnim(receiver_anim, tiny);
			}
		}
	}

}

namespace GTS {

	std::vector<Actor*> ThighSandwichAI_FilterList(Actor* a_Performer, const std::vector<Actor*>& a_PotentialPrey) {

		if (!a_Performer) {
			return {};
		}

		if (AnimationVars::Crawl::IsCrawling(a_Performer)) {
			return {};
		}

		const auto CharacterController = a_Performer->GetCharController();
		if (!CharacterController) {
			return {};
		}

		const NiPoint3 PredPosition = a_Performer->GetPosition();

		auto PreyList = a_PotentialPrey;

		// Sort prey by distance
		std::ranges::sort(PreyList,[PredPosition](const Actor* a_PreyA, const Actor* a_PreyB) -> bool {
			const float DistToA = (a_PreyA->GetPosition() - PredPosition).Length();
			const float DistToB = (a_PreyB->GetPosition() - PredPosition).Length();
			return DistToA < DistToB;
		});

		// Filter out invalid targets
		std::erase_if(PreyList, [a_Performer](auto idxPrey) {
			return !CanSandwich(a_Performer, idxPrey);
		});

		// Filter out actors not in front
		const auto ActorAngle = a_Performer->data.angle.z;
		constexpr NiPoint3 FWDVector{ 0.f, 1.f, 0.f };
		const NiPoint3 ActorForward = RotateAngleAxis(FWDVector, -ActorAngle, { 0.f, 0.f, 1.f });

		NiPoint3 PredDirection = ActorForward;
		PredDirection = PredDirection / PredDirection.Length();
		std::erase_if(PreyList, [PredPosition, PredDirection](auto idxPrey) {
			NiPoint3 PreyDirection = idxPrey->GetPosition() - PredPosition;
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
		float ShiftAmount = fabs((PredConeWidth / 2.0f) / tan(SANDWICH_ANGLE / 2.0f));

		const NiPoint3 ConeStart = PredPosition - PredDirection * ShiftAmount;
		std::erase_if(PreyList, [ConeStart, PredConeWidth, PredDirection](auto prey) {
			NiPoint3 PreyDirection = prey->GetPosition() - ConeStart;
			if (PreyDirection.Length() <= PredConeWidth * 0.4f) {
				return false;
			}
			PreyDirection = PreyDirection / PreyDirection.Length();
			const float CosineTheta = PredDirection.Dot(PreyDirection);
			return CosineTheta <= cos(SANDWICH_ANGLE * PI / 180.0f);
		});

		return GetMaxActionableTinyCount(a_Performer, PreyList);
	}


	//This gets called by SandwichingData::Update() in Thighsandwichcontroller.cpp
	void ThighSandwichAI_DecideAction(Actor* a_Performer, bool a_HavePrey) {

		bool CanSandwich = a_HavePrey && GetPercentageAV(a_Performer, ActorValue::kStamina) > 0.05f;
		const auto& Settings = Config::AI.ThighSandwich;
		bool IsSecondBranchThighGrinding = AnimationVars::Action::IsThighGrinding(a_Performer);
		bool IsInSecondBranch = GTS::AnimationVars::Action::IsInSecondSandwichBranch(a_Performer) || IsSecondBranchThighGrinding;
		int AbortModifier = (static_cast<int>(!CanSandwich) * 100000);

		if (IsInSecondBranch) {
			
			//Thigh grind within second branch
			if (IsSecondBranchThighGrinding) {
				//Massively skew stop chance in case we can no longer grind.
				switch (RandomIntWeighted({static_cast<int>(Settings.fButtGrindStop) + AbortModifier , 100 })) {

					case 0: 
					{
						AnimationManager::StartAnim("TinyDied", a_Performer);
					} return;
					
					default: 
					{
						//Do nothing
					} return;
				}
			}

			//Second Branch Base
			switch (RandomIntWeighted({
					static_cast<int>(Settings.fButtLAtkProb),
					static_cast<int>(Settings.fButtHAtkProb),
					static_cast<int>(Settings.fButtGrowProb),
					static_cast<int>(Settings.fButtGrindStart),
					static_cast<int>(Settings.fButtVoreProb),
					static_cast<int>(Settings.fButtExitProb) + AbortModifier,
					100
				})) {

				case 0: //Light Attack
				{
					PerformAnimations(a_Performer, "Sandwich_LightAttack", "Sandwich_LightAttack_T");
				} return;

				case 1: //Heavy Attack
				{
					PerformAnimations(a_Performer, "Sandwich_HeavyAttack", "Sandwich_HeavyAttack_T");
				} return;

				case 2: //Grow
				{
					PerformAnimations(a_Performer, "Sandwich_Grow", "Sandwich_Grow_T");
				} return;

				case 3: //Grind Start
				{
					PerformAnimations(a_Performer, "Sandwich_GrindStart", "Sandwich_GrindStart_T");
				} return;

				case 4: //UB
				{
					PerformAnimations(a_Performer, "Sandwich_UB", "Sandwich_UB_T");
				} return;

				case 5: //Stop Butt
				{
					PerformAnimations(a_Performer, "Sandwich_ButtStop", "Sandwich_ButtStop_T");
				} return;

				default: 
				{
					//Nothing
				} return;
			}
		}
		else {
			if (CanSandwich) {
				

				switch (RandomIntWeighted({
						static_cast<int>(Settings.fLightAttackProb),
						static_cast<int>(Settings.fHeavyAttackPob),
						static_cast<int>(Settings.fEnterButtModeProb),
						100
					})) {

					case 0: 
					{
						AnimationManager::StartAnim("ThighAttack", a_Performer);
					} return;
					case 1: 
					{
						AnimationManager::StartAnim("ThighAttack_Heavy", a_Performer);
					} return;
					case 2: 
					{
						auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(a_Performer);
						if (!sandwichdata.GetActors().empty()) {
							PerformAnimations(a_Performer, "Sandwich_ButtStart", "Sandwich_ButtStart_T");
						}
					} return;
					default: 
					{
						//Nothing
					} return;
				}
			}
			else {
				AnimationManager::StartAnim("ThighExit", a_Performer);
			}
		}
	}

	void ThighSandwichAI_Start(Actor* a_Performer, const std::vector<Actor*>& a_PreyList) {
		for (const auto& prey : a_PreyList) {
			ThighSandwichController::StartSandwiching(a_Performer, prey, false);
			auto node = find_node(a_Performer, "GiantessRune", false);
			if (node) {
				node->local.scale = 0.01f;
				update_node(node);
			}
		}
	}
}
