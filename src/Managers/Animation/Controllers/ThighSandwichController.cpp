#include "Managers/Animation/Controllers/ThighSandwichController.hpp"

#include "Managers/Animation/AnimationManager.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/AttachPoint.hpp"

#include "Managers/GTSSizeManager.hpp"
#include "Magic/Effects/Common.hpp"

#include "Managers/AI/Thigh/ThighSandwichAI.hpp"
#include "Managers/Animation/Utils/TurnTowards.hpp"

#include "Utils/DeathReport.hpp"

using namespace GTS;


namespace {
	constexpr float MINIMUM_SANDWICH_DISTANCE = 70.0f;
	constexpr float SANDWICH_ANGLE = 60;
	constexpr float PI = std::numbers::pi_v<float>;

	const string rune_node = "GiantessRune";

	void CantThighSandwichPlayerMessage(Actor* giant, Actor* tiny, float sizedifference) {
		if (sizedifference < Action_Sandwich) {
			std::string message = std::format("Player is too big to be sandwiched: x{:.2f}/{:.2f}", sizedifference, Action_Sandwich);
			NotifyWithSound(tiny, message);
		}
	}

	void EnlargeRuneTask(Actor* a_Giant) {
		double Start = Time::WorldTimeElapsed();
		std::string name = std::format("ShrinkRune_{}", a_Giant->formID);
		ActorHandle gianthandle = a_Giant->CreateRefHandle();

		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			double Finish = Time::WorldTimeElapsed();
			auto giantref = gianthandle.get().get();
			auto node = find_node(giantref, rune_node);
			double timepassed = std::clamp(((Finish - Start) * GetAnimationSlowdown(giantref)) * 0.70, 0.01, 1.0);
			if (node) {
				node->local.scale = static_cast<float>(std::clamp(timepassed, 0.01, 1.0));
				update_node(node);
			}
			if (timepassed >= 0.98 || !AnimationVars::General::IsGTSBusy(giantref)) {
				return false; // end it
			}
			return true;
		});
	}

	void ShrinkRuneTask(Actor* a_Giant) {
		double Start = Time::WorldTimeElapsed();
		std::string name = std::format("ScaleRune_{}", a_Giant->formID);
		ActorHandle gianthandle = a_Giant->CreateRefHandle();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			double Finish = Time::WorldTimeElapsed();
			auto giantref = gianthandle.get().get();
			auto node = find_node(giantref, rune_node);
			double timepassed = std::clamp(((Finish - Start) * GetAnimationSlowdown(giantref)) * 0.80, 0.01, 9999.0);
			//log::info("Grow Rune task is running, timepassed: {}, AnimationSlowdown: {} ", timepassed, GetAnimationSlowdown(giantref));
			if (node) {
				node->local.scale = static_cast<float>(std::clamp(1.0 - timepassed, 0.005, 1.0));
				update_node(node);
			}
			if (!AnimationVars::General::IsGTSBusy(giantref)) {
				return false; // end it
			}
			return true;
		});
	}
}

namespace GTS {

	SandwichingData::SandwichingData(Actor* giant) : giant(giant? giant->CreateRefHandle() : ActorHandle()) {}

	void SandwichingData::AddTiny(Actor* tiny) {
		this->tinies.try_emplace(tiny->formID, tiny->CreateRefHandle());
	}

	std::vector<Actor*> SandwichingData::GetActors() {
		std::vector<Actor*> result;
		for (auto& actorref : this->tinies | views::values) {
			auto actor = actorref.get().get();
			result.push_back(actor);
		}
		return result;
	}

	std::string ThighSandwichController::DebugName() {
		return "::ThighSandwichController";
	}

	void SandwichingData::MoveActors(bool move) {
		this->MoveTinies = move;
	}

	void SandwichingData::StartRuneTask(Actor* a_Giant, RuneTask Type) {
		switch (Type) {
			case RuneTask::kEnlarge:
				EnlargeRuneTask(a_Giant);
			break;
			case RuneTask::kShrink:
				ShrinkRuneTask(a_Giant);
			break;
		}
	}

	void SandwichingData::Update() {

		if (this->giant) {
			auto GiantRef = this->giant.get().get();
			bool MoveTinies = this->MoveTinies;

			if (!GiantRef) {
				return;
			}

			const bool AttachToR = Attachment_GetTargetNode(GiantRef) == AttachToNode::ObjectR;
			float giantScale = get_visual_scale(GiantRef);
			

			//If AI
			if ((!GiantRef->IsPlayerRef()) || (GiantRef->IsPlayerRef() && Config::Advanced.bPlayerAI)) {

				if (auto AITransientData = Transient::GetActorData(GiantRef)) {
					AITransientData->ActionTimer.UpdateDelta(Config::AI.ThighSandwich.fInterval);

					if (!State::Live()) return;

					if (GetPlayerOrControlled()->IsPlayerRef() && AITransientData->ActionTimer.ShouldRunFrame()) {
						ThighSandwichAI_DecideAction(GiantRef, tinies.size() > 0);
					}
				}
			}

			for (auto& tinyref : this->tinies | views::values) {

				if (!MoveTinies) {
					return;
				}

				auto tiny = tinyref.get().get();
				if (!tiny) {
					return;
				}

				Actor* tiny_is_actor = skyrim_cast<Actor*>(tiny);
				if (tiny_is_actor) {
					AttachToR ? AttachToObjectR(GiantRef, tiny_is_actor) : AttachToObjectA(GiantRef, tiny_is_actor);
					//ForceRagdoll(tiny_is_actor, false);
					ShutUp(tiny_is_actor);
					FaceOpposite(GiantRef, tiny_is_actor);
				}

				float tinyScale = get_visual_scale(tiny);
				float sizedifference = get_scale_difference(GiantRef, tiny, SizeType::VisualScale, true, false);
				float threshold = Action_Sandwich;

				if (GiantRef->IsDead() || sizedifference < threshold || !AnimationVars::Action::IsThighSandwiching(GiantRef)) {
					Attachment_SetTargetNode(GiantRef, AttachToNode::None);

					EnableCollisions(tiny);
					SetBeingHeld(tiny, false);
					AllowToBeCrushed(tiny, true);
					PushActorAway(GiantRef, tiny, 1.0f);
					ForceRagdoll(tiny_is_actor, true);
					Cprint("{} slipped out of {} thighs", tiny->GetDisplayFullName(), GiantRef->GetDisplayFullName());
					this->tinies.erase(tiny->formID); // Disallow button abuses to keep tiny when on low scale
				}

				if (this->Suffocate && CanDoDamage(GiantRef, tiny, false)) {
					sizedifference = giantScale/tinyScale;
					float damage = Damage_ThighSandwich_DOT * sizedifference * TimeScale();
					float hp = GetAV(tiny, ActorValue::kHealth);
					InflictSizeDamage(GiantRef, tiny, damage);
					if (damage > hp && !tiny->IsDead()) {
						ReportDeath(GiantRef, tiny, DamageSource::ThighSuffocated);
						Attachment_SetTargetNode(GiantRef, AttachToNode::None);
						this->Remove(tiny);
					}
				}
			}
		}
	}

	void ThighSandwichController::Update() {
		for (auto& SandwichData : this->data | views::values) {
			SandwichData.Update();
		}
	}

	std::vector<Actor*> ThighSandwichController::GetSandwichTargetsInFront(Actor* pred, std::size_t numberOfPrey) {
		// Get vore target for actor
		auto& sizemanager = SizeManager::GetSingleton();
		if (!CanDoActionBasedOnQuestProgress(pred, QuestAnimationType::kGrabAndSandwich)) {
			return {};
		}
		if (AnimationVars::General::IsGTSBusy(pred)) {
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
		ranges::sort(preys,[predPos](const Actor* preyA, const Actor* preyB) -> bool{
			float distanceToA = (preyA->GetPosition() - predPos).Length();
			float distanceToB = (preyB->GetPosition() - predPos).Length();
			return distanceToA < distanceToB;
		});

		// Filter out invalid targets
		std::erase_if(preys,[pred, this](auto prey){
			return !this->CanSandwich(pred, prey);
		});

		// Filter out actors not in front
		auto actorAngle = pred->data.angle.z;
		RE::NiPoint3 forwardVector{ 0.f, 1.f, 0.f };
		RE::NiPoint3 actorForward = RotateAngleAxis(forwardVector, -actorAngle, { 0.f, 0.f, 1.f });

		NiPoint3 predDir = actorForward;
		predDir = predDir / predDir.Length();
		std::erase_if(preys,[predPos, predDir](auto prey){
			NiPoint3 preyDir = prey->GetPosition() - predPos;
			if (preyDir.Length() <= 1e-4) {
				return false;
			}
			preyDir = preyDir / preyDir.Length();
			float cosTheta = predDir.Dot(preyDir);
			return cosTheta <= 0; // 180 degress
		});

		// Filter out actors not in a truncated cone
		// \      x   /
		//  \  x     /
		//   \______/  <- Truncated cone
		//   | pred |  <- Based on width of pred
		//   |______|
		float predWidth = 70 * get_visual_scale(pred);
		float shiftAmount = fabs((predWidth / 2.0f) / tan(SANDWICH_ANGLE/2.0f));

		NiPoint3 coneStart = predPos - predDir * shiftAmount;
		std::erase_if(preys,[coneStart, predWidth, predDir](auto prey){
			NiPoint3 preyDir = prey->GetPosition() - coneStart;
			if (preyDir.Length() <= predWidth*0.4f) {
				return false;
			}
			preyDir = preyDir / preyDir.Length();
			float cosTheta = predDir.Dot(preyDir);
			return cosTheta <= cos(SANDWICH_ANGLE*PI/180.0f);
		});

		if (numberOfPrey == 1) {
			return GetMaxActionableTinyCount(pred, preys);
		}
		
		// Reduce vector size
		if (preys.size() > numberOfPrey) {
			preys.resize(numberOfPrey);
		}

		return preys;
	}

	bool ThighSandwichController::CanSandwich(Actor* pred, Actor* prey) const {
		if (pred == prey) {
			return false;
		}

		if (prey->IsDead()) {
			return false;
		}

		if (IsBeingHeld(pred, prey)) {
			return false;
		}

		float pred_scale = get_visual_scale(pred);

		float sizedifference = get_scale_difference(pred, prey, SizeType::VisualScale, true, false);

		float MINIMUM_SANDWICH_SCALE = Action_Sandwich;

		float MINIMUM_DISTANCE = MINIMUM_SANDWICH_DISTANCE;

		if (HasSMT(pred)) {
			MINIMUM_DISTANCE *= 1.75f;
		}

		float prey_distance = (pred->GetPosition() - prey->GetPosition()).Length();
		if (prey_distance <= (MINIMUM_DISTANCE * pred_scale) && sizedifference < MINIMUM_SANDWICH_SCALE) {
			if (pred->IsPlayerRef()) {
				std::string_view message = fmt::format("{} is too big to be smothered between thighs: x{:.2f}/{:.2f}", prey->GetDisplayFullName(), sizedifference, MINIMUM_SANDWICH_SCALE);
				shake_camera(pred, 0.45f, 0.30f);
				NotifyWithSound(pred, message);
			} else if (this->allow_message && prey->IsPlayerRef() && IsTeammate(pred)) {
				CantThighSandwichPlayerMessage(pred, prey, sizedifference);
			}
			return false;
		}
		if (prey_distance <= (MINIMUM_DISTANCE * pred_scale) && sizedifference > MINIMUM_SANDWICH_SCALE) {
			if ((!prey->IsPlayerRef() && IsEssential(pred, prey))) {
				return false;
			} else {
				return true;
			}
		} else {
			return false;
		}
	}

	void ThighSandwichController::StartSandwiching(Actor* pred, Actor* prey, bool dochecks) {
		auto& sandwiching = ThighSandwichController::GetSingleton();

		if (dochecks) {
			if (!sandwiching.CanSandwich(pred, prey)) {
				return;
			}
		}

		if (IsBeingHeld(pred, prey)) {
			return;
		}
		
		if (get_scale_difference(pred, prey, SizeType::VisualScale, false, false) < Action_Sandwich) {
			ShrinkUntil(pred, prey, 6.0f, 0.20f, true);
			return;
		}
		
		auto& data = sandwiching.GetSandwichingData(pred);
		data.AddTiny(prey);
		//BlockFirstPerson(pred, true);
		AnimationManager::StartAnim("ThighEnter", pred);
	}

	void ThighSandwichController::Reset() {
		this->data.clear();
	}

	void SandwichingData::ReleaseAll() {
		this->tinies.clear();
		this->MoveTinies = false;
	}

	void ThighSandwichController::ResetActor(Actor* actor) {
		this->data.erase(actor->formID);
	}

	void SandwichingData::Remove(Actor* tiny) {
		this->tinies.erase(tiny->formID);
	}

	void SandwichingData::EnableSuffocate(bool enable) {
		this->Suffocate = enable;
	}

	SandwichingData& ThighSandwichController::GetSandwichingData(Actor* giant) {
		// Create it now if not there yet
		this->data.try_emplace(giant->formID, giant);
		return this->data.at(giant->formID);
	}

	void ThighSandwichController::AllowMessage(bool allow) {
		this->allow_message = allow;
	}
}
