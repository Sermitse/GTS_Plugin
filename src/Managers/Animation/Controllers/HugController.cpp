#include "Managers/Animation/Controllers/HugController.hpp"

#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"

#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/HugShrink.hpp"

#include "Managers/GTSSizeManager.hpp"

using namespace GTS;

namespace {

	constexpr float MINIMUM_HUG_DISTANCE = 110.0f;
	constexpr float GRAB_ANGLE = 70.0f;
	constexpr float PI = std::numbers::pi_v<float>;;

	bool DisallowHugs(Actor* actor) {
		bool jumping = IsJumping(actor);
		bool ragdolled = IsRagdolled(actor);
		bool busy = IsGtsBusy(actor);
		return jumping || ragdolled || busy;
	}

	void CantHugPlayerMessage(Actor* giant, Actor* tiny, float sizedifference, bool allow) {
		if (allow) {
			if (sizedifference < Action_Hug) {
				std::string message = std::format("Player is too big for hugs: x{:.2f}/{:.2f}", sizedifference, Action_Hug);
				NotifyWithSound(tiny, message);
			} else if (sizedifference > GetHugShrinkThreshold(giant)) {
				std::string message = std::format("Player is too small for hugs: x{:.2f}/{:.2f}", sizedifference, GetHugShrinkThreshold(giant));
				NotifyWithSound(tiny, message);
			}
		}
	}

	bool ShouldAllowWhenTooLarge(Actor* giant, Actor* tiny, float sizedifference, bool allow) {
		if (giant->formID != 0x14 && IsTeammate(giant) && sizedifference > GetHugShrinkThreshold(giant)) {
			// Disallow FOLLOWERS to hug someone when size difference is too massive
			if (tiny->formID == 0x14) {
				CantHugPlayerMessage(giant, tiny, sizedifference, allow);
			}
			return false;
		}
		return true;
	}

	void RecordSneakState(Actor* giant, Actor* tiny) {
		bool Crawling = IsCrawling(giant);
		bool Sneaking = giant->IsSneaking();

		tiny->SetGraphVariableBool("GTS_Hug_Sneak_Tny", Sneaking); // Is Sneaking?
		tiny->SetGraphVariableBool("GTS_Hug_Crawl_Tny", Crawling); // Is Crawling?
	}

	void Task_PerformHugs(Actor* giant, Actor* tiny) {
		std::string taskname = std::format("PerformHugs_{}_{}", giant->formID, tiny->formID);
		ActorHandle giantHandle = giant->CreateRefHandle();
		ActorHandle tinyHandle = tiny->CreateRefHandle();
		TaskManager::RunOnce(taskname, [=](auto& update){
			if (!tinyHandle) {
				return;
			}
			if (!giantHandle) {
				return;
			}

			auto pred = giantHandle.get().get();
			auto prey = tinyHandle.get().get();
			
			HugShrink::GetSingleton().HugActor(pred, prey);
			AnimationManager::StartAnim("Huggies_Try", pred);

			if (pred->IsSneaking()) {
				if (!IsCrawling(pred)) {
					SetSneaking(pred, true, 0); // If just sneaking, disable sneaking so footstep sounds will work properly
				}
				AnimationManager::StartAnim("Huggies_Try_Victim_S", prey); // GTSBEH_HugAbsorbStart_Sneak_V
			} else {
				AnimationManager::StartAnim("Huggies_Try_Victim", prey); //   GTSBEH_HugAbsorbStart_V
			}

			ApplyActionCooldown(pred, CooldownSource::Action_Hugs);
		});
	}
}

namespace GTS {

	void HugAnimationController::Hugs_OnCooldownMessage(Actor* giant) {
		double cooldown = GetRemainingCooldown(giant, CooldownSource::Action_Hugs);
		if (giant->formID == 0x14) {
			std::string message = std::format("Hugs are on a cooldown: {:.1f} sec", cooldown);
			shake_camera(giant, 0.75f, 0.35f);
			NotifyWithSound(giant, message);
		} else if (IsTeammate(giant) && !IsGtsBusy(giant)) {
			std::string message = std::format("Follower's Hugs are on a cooldown: {:.1f} sec", cooldown);
			NotifyWithSound(giant, message);
		}
	}
	


	std::vector<Actor*> HugAnimationController::GetHugTargetsInFront(Actor* pred, std::size_t numberOfPrey) {
		// Get vore target for actor
		auto& sizemanager = SizeManager::GetSingleton();
		if (IsGtsBusy(pred)) {
			return {};
		}
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
			return !this->CanHug(pred, prey);
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
		float shiftAmount = fabs((predWidth / 2.0f) / tan(GRAB_ANGLE/2.0f));

		NiPoint3 coneStart = predPos - predDir * shiftAmount;
		preys.erase(std::remove_if(preys.begin(), preys.end(),[coneStart, predWidth, predDir](auto prey)
		{
			NiPoint3 preyDir = prey->GetPosition() - coneStart;
			if (preyDir.Length() <= predWidth*0.4f) {
				return false;
			}
			preyDir = preyDir / preyDir.Length();
			float cosTheta = predDir.Dot(preyDir);
			return cosTheta <= cos(GRAB_ANGLE*PI/180.0f);
		}), preys.end());

		// Reduce vector size
		if (preys.size() > numberOfPrey) {
			preys.resize(numberOfPrey);
		}

		return preys;
	}

	bool HugAnimationController::CanHug(Actor* pred, Actor* prey) {
		if (pred == prey) {
			return false;
		}

		if (prey->IsDead()) {
			return false;
		}
		if (IsTransitioning(pred) || IsBeingHeld(pred, prey)) {
			return false;
		}
		if (DisallowHugs(pred) || DisallowHugs(prey)) {
			return false;
		}

		if (pred->AsActorState()->GetSitSleepState() == SIT_SLEEP_STATE::kIsSitting) { // disallow doing it when using furniture
			return false;	
		}

		float pred_scale = get_visual_scale(pred);
		// No need to check for BB scale in this case

		float sizedifference = GetSizeDifference(pred, prey, SizeType::VisualScale, false, true);
		

		float MINIMUM_DISTANCE = MINIMUM_HUG_DISTANCE;
		float MINIMUM_HUG_SCALE = Action_Hug;

		if (pred->IsSneaking()) {
			if (IsCrawling(pred)) {
				MINIMUM_DISTANCE *= 2.35f;
			} else {
				MINIMUM_DISTANCE *= 1.6f;
			}
		}
	
		if (HasSMT(pred)) {
			MINIMUM_HUG_SCALE *= 0.80f;
		}

		float prey_distance = (pred->GetPosition() - prey->GetPosition()).Length();

		if (prey_distance <= (MINIMUM_DISTANCE * pred_scale)) {
			if (sizedifference > MINIMUM_HUG_SCALE) {
				if ((prey->formID != 0x14 && !CanPerformAnimationOn(pred, prey, true))) {
					return false;
				}
				if (!IsHuman(prey)) { // Allow hugs with humanoids only
					if (pred->formID == 0x14) {
						std::string_view message = std::format("You have no desire to hug {}", prey->GetDisplayFullName());
						NotifyWithSound(pred, message); // Just no. We don't have Creature Anims.
						shake_camera(pred, 0.45f, 0.30f);
					}
					return false;
				}
				return ShouldAllowWhenTooLarge(pred, prey, sizedifference, this->allow_message);
			} else {
				if (pred->formID == 0x14) {
					std::string_view message = std::format("{} is too big to be hugged: x{:.2f}/{:.2f}", prey->GetDisplayFullName(), sizedifference, MINIMUM_HUG_SCALE);
					shake_camera(pred, 0.45f, 0.30f);
					NotifyWithSound(pred, message);
				} else if (prey->formID == 0x14 && IsTeammate(pred)) {
					CantHugPlayerMessage(pred, prey, sizedifference, this->allow_message);
				}
				return false;
			}
			return false;
		}
		return false;
	}

	void HugAnimationController::StartHug(Actor* pred, Actor* prey) {
		auto& hugging = HugAnimationController::GetSingleton();
		if (!hugging.CanHug(pred, prey)) {
			return;
		}

		if (IsActionOnCooldown(pred, CooldownSource::Action_Hugs)) {
			HugAnimationController::Hugs_OnCooldownMessage(pred);
			return;
		}

		if (IsCrawling(pred)) {
			if (!CanPerformAnimation(PlayerCharacter::GetSingleton(), AnimationCondition::kOthers)) {
				// Can Crawl Hug only after quest is done
				if (pred->formID == 0x14) {
					NotifyWithSound(pred, "You're not experienced enough for Crawl Hugs");
				}
				return;
			}
			DamageAV(pred, ActorValue::kMagicka, 225 * Perk_GetCostReduction(pred));
		}

		UpdateFriendlyHugs(pred, prey, false);
		RecordSneakState(pred, prey); // Helps to determine which hugs to play: normal or crawl ones
		RecordSneaking(pred); // store previous sneak value, used to fix missing footstep sounds in sneak later
		DisarmActor(prey, false);

		Task_PerformHugs(pred, prey); // Start hugs
	}

	void HugAnimationController::AllowMessage(bool allow) {
		this->allow_message = allow;
	}
}