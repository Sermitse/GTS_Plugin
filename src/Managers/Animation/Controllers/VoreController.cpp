#include "Managers/Animation/Controllers/VoreController.hpp"
#include "Managers/Animation/Utils/AttachPoint.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Perks/PerkHandler.hpp"
#include "Managers/AI/AIFunctions.hpp"
#include "Magic/Effects/Common.hpp"
#include "Utils/SurvivalMode.hpp"
#include "Utils/VoreUtils.hpp"
#include "Utils/Looting.hpp"

#include "Utils/DeathReport.hpp"

using namespace GTS;

namespace {
	constexpr float MINIMUM_VORE_DISTANCE = 95.0f;
	constexpr float VORE_ANGLE = 75;
	constexpr float PI = std::numbers::pi_v<float>;
}

namespace GTS {

	VoreData::VoreData(Actor* giant) : giant(giant? giant->CreateRefHandle() : ActorHandle()) {}

	void VoreData::AddTiny(Actor* tiny) {
		std::unique_lock lock(_lock);
		this->tinies.try_emplace(tiny->formID, tiny->CreateRefHandle());
	}

	void VoreData::EnableMouthShrinkZone(bool enabled) {
		this->killZoneEnabled = enabled;
	}

	void VoreData::Swallow() {
		std::unique_lock lock(_lock);
		for (auto& [key, tinyref]: this->tinies) {
			auto tiny = tinyref.get().get();
			auto giant = this->giant.get().get();
			
			if (giant->formID == 0x14) {
				if (IsLiving(tiny) && IsHuman(tiny)) {
					CallVampire();
				}
				bool Living = IsLiving(tiny);
				
				float DefaultScale = get_natural_scale(tiny);
				ModSizeExperience(giant, 0.08f + (DefaultScale*0.025f));

				SurvivalMode_AdjustHunger(this->giant.get().get(), VoreController::ReadOriginalScale(tiny) * GetSizeFromBoundingBox(tiny), Living, false);
			}

			Task_Vore_StartVoreBuff(giant, tiny, static_cast<int>(this->tinies.size()));
			ReportDeath(giant, tiny, DamageSource::Vored, false);
		}
	}
	void VoreData::KillAll() {
		std::unique_lock lock(_lock);
		if (!AllowDevourment()) {

			for (auto& tinyref : this->tinies | views::values) {
				auto tiny = tinyref.get().get();
				auto giantref = this->giant;
				SetBeingHeld(tiny, false);
				AddSMTDuration(giantref.get().get(), 6.0f);

				const auto& MuteVore = Config::Audio.bMuteVoreDeathScreams;

				if (tiny->formID != 0x14) {
					KillActor(giantref.get().get(), tiny, MuteVore);
					PerkHandler::UpdatePerkValues(giantref.get().get(), PerkUpdate::Perk_LifeForceAbsorption);
				}
				else if (tiny->formID == 0x14) {
					InflictSizeDamage(giantref.get().get(), tiny, 900000);
					KillActor(giantref.get().get(), tiny, MuteVore);
					TriggerScreenBlood(50);
					tiny->SetAlpha(0.0f); // Player can't be disintegrated: simply nothing happens. So we Just make player Invisible instead.
				}

				Vore_AdvanceQuest(giantref.get().get(), tiny, IsDragon(tiny), IsGiant(tiny)); // Progress quest
				DecreaseShoutCooldown(giantref.get().get());

				std::string taskname = std::format("VoreAbsorb_{}", tiny->formID);

				TaskManager::RunOnce(taskname, [=](auto& update) {
					if (!tinyref) {
						return;
					}
					if (!giantref) {
						return;
					}
					auto giant = giantref.get().get();
					auto smoll = tinyref.get().get();

					if (smoll->formID != 0x14) {
						Disintegrate(smoll);
					}
					TransferInventory(smoll, giant, 1.0f, false, true, DamageSource::Vored, true);
				});
			}
		} else {
			for (auto& [key, tinyref]: this->tinies) { // just clear the data
				auto tiny = tinyref.get().get();
				SetBeingHeld(tiny, false);
			}
		}
		this->tinies.clear();
	}

	void VoreData::AllowToBeVored(bool allow) {
		std::unique_lock lock(_lock);
		for (auto& [key, tinyref]: this->tinies) {
			auto tiny = tinyref.get().get();
			auto transient = Transient::GetSingleton().GetData(tiny);
			if (transient) {
				transient->CanBeVored = allow;
			}
		}
	}

	bool VoreData::GetTimer() {
		return this->moantimer.ShouldRun();
	}

	void VoreData::GrabAll() {
		this->allGrabbed = true;
	}

	void VoreData::ReleaseAll() {
		this->allGrabbed = false;
	}

	std::vector<Actor*> VoreData::GetVories() {
		std::unique_lock lock(_lock);
		std::vector<Actor*> result;
		for (auto& actorref : this->tinies | views::values) {
			auto actor = actorref.get().get();
			result.push_back(actor);
		}
		return result;
	}

	void VoreData::Update() {
		GTS_PROFILE_SCOPE("VoreData: Update");
		if (this->giant) {
			auto giant = this->giant.get().get();
			float giantScale = get_visual_scale(giant);
			// Stick them to the AnimObjectA
			for (auto& [key, tinyref]: this->tinies) {
				auto tiny = tinyref.get().get();
				if (!tiny) {
					return;
				}
				if (!giant) {
					return;
				}

				if (this->allGrabbed && !giant->IsDead()) {
					AttachToObjectA(giant, tiny);
				}
			}
		}
	}

	VoreController& VoreController::GetSingleton() noexcept {
		static VoreController instance;
		return instance;
	}

	std::string VoreController::DebugName() {
		return "::VoreController";
	}

	void VoreController::Update() {
		std::unique_lock lock(_lock);
		for (auto& voreData : this->data | views::values) {
			voreData.Update();
		}
	}

	Actor* VoreController::GetVoreTargetInFront(Actor* pred) {
		auto preys = this->GetVoreTargetsInFront(pred, 1);
		if (!preys.empty()) {
			return preys[0];
		} else {
			return nullptr;
		}
	}

	std::vector<Actor*> VoreController::GetVoreTargetsInFront(Actor* pred, std::size_t numberOfPrey) {
		// Get vore target for actor
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
			return !this->CanVore(pred, prey);
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
		float shiftAmount = fabs((predWidth / 2.0f) / tan(VORE_ANGLE/2.0f));

		NiPoint3 coneStart = predPos - predDir * shiftAmount;
		preys.erase(std::remove_if(preys.begin(), preys.end(),[coneStart, predWidth, predDir](auto prey)
		{
			NiPoint3 preyDir = prey->GetPosition() - coneStart;
			if (preyDir.Length() <= predWidth*0.4f) {
				return false;
			}
			preyDir = preyDir / preyDir.Length();
			float cosTheta = predDir.Dot(preyDir);
			return cosTheta <= cos(VORE_ANGLE*PI/180.0f);
		}), preys.end());

		if (numberOfPrey == 1) {
			return GetMaxActionableTinyCount(pred, preys);
		}

		// Reduce vector size
		if (preys.size() > numberOfPrey) {
			preys.resize(numberOfPrey);
		}

		return preys;
	}

	bool VoreController::CanVore(Actor* pred, Actor* prey) {

		if (pred == prey) {
			return false;
		}

		if (!CanPerformAnimation(pred, AnimationCondition::kVore)) {
			return false;
		}

		auto transient = Transient::GetSingleton().GetData(prey);
		if (prey->IsDead()) {
			return false;
		}

		if (IsBeingHeld(pred, prey)) {
			return false;
		}

		if (transient) {
			if (transient->CanBeVored == false) {
				Notify("{} is already being eaten by someone else", prey->GetDisplayFullName());
				Cprint("{} is already being eaten by someone else", prey->GetDisplayFullName());
				return false;
			}
		}

		float MINIMUM_VORE_SCALE = Action_Vore;
		float MINIMUM_DISTANCE = MINIMUM_VORE_DISTANCE;

		if (HasSMT(pred)) {
			MINIMUM_DISTANCE *= 1.75f;
		}

		float pred_scale = get_visual_scale(pred);
		float sizedifference = GetSizeDifference(pred, prey, SizeType::VisualScale, true, false);
		float prey_distance = (pred->GetPosition() - prey->GetPosition()).Length();

		

		if (prey_distance <= (MINIMUM_DISTANCE * pred_scale) && sizedifference < MINIMUM_VORE_SCALE) {
			if (IsInsect(prey, true) || IsBlacklisted(prey) || IsUndead(prey, true)) {
				std::string_view message = fmt::format("{} has no desire to eat {}", pred->GetDisplayFullName(), prey->GetDisplayFullName());
				NotifyWithSound(pred, message);
				return false;
			}
			
			if (pred->formID == 0x14) {
				std::string_view message = fmt::format("{} is too big to be eaten: x{:.2f}/{:.2f}", prey->GetDisplayFullName(), sizedifference, MINIMUM_VORE_SCALE);
				shake_camera(pred, 0.45f, 0.30f);
				NotifyWithSound(pred, message);
			} else if (this->allow_message && prey->formID == 0x14 && IsTeammate(pred)) {
				CantVorePlayerMessage(pred, prey, sizedifference);
			}
			return false;
		}

		if (prey_distance <= (MINIMUM_DISTANCE * pred_scale) && sizedifference > MINIMUM_VORE_SCALE) {
			if (IsFlying(prey)) {
				return false; // Disallow to vore flying dragons
			}
			if ((prey->formID != 0x14 && !CanPerformAnimationOn(pred, prey, false))) {
				Notify("{} is important and shouldn't be eaten.", prey->GetDisplayFullName());
				return false;
			}
			else {
				return true;
			}
		}
		else {
			return false;
		}
	}

	void VoreController::Reset() {
		std::unique_lock lock(_lock);
		this->data.clear();
	}

	void VoreController::ResetActor(Actor* actor) {
		std::unique_lock lock(_lock);
		this->data.erase(actor->formID);
	}

	void VoreController::StartVore(Actor* pred, Actor* prey) {

		if (!CanVore(pred, prey)) {
			return;
		}

		float pred_scale = get_visual_scale(pred);
		float prey_scale = get_visual_scale(prey);

		float sizedifference = pred_scale/prey_scale;

		float wastestamina = 45; // Drain stamina, should be 300 once tests are over
		float staminacheck = pred->AsActorValueOwner()->GetActorValue(ActorValue::kStamina);

		if (pred->formID != 0x14) {
			wastestamina = 30; // Less tamina drain for non Player
		}

		if (!Runtime::HasPerkTeam(pred, "GTSPerkVoreAbility")) { // Damage stamina if we don't have perk
			if (staminacheck < wastestamina) {
				Notify("{} is too tired for vore.", pred->GetDisplayFullName());
				DamageAV(prey, ActorValue::kHealth, 3 * sizedifference);
				if (pred->formID == 0x14) {
					Runtime::PlaySound("GTSSoundFail", pred, 0.4f, 1.0f);
				}
				StaggerActor(pred, prey, 0.25f);
				return;
			}
			DamageAV(pred, ActorValue::kStamina, wastestamina);
		}

		
		if (GetSizeDifference(pred, prey, SizeType::VisualScale, false, false) < Action_Vore) {
			if (pred->IsSneaking() && !IsCrawling(pred)) {
				ShrinkUntil(pred, prey, 10.2f, 0.14f, true); // Shrink if we have SMT to allow 'same-size' vore
			} else {
				ShrinkUntil(pred, prey, 10.2f, 0.16f, true); // Shrink if we have SMT to allow 'same-size' vore
				StaggerActor(pred, prey, 0.25f);
			}
			return;
		}

		if (pred->formID == 0x14) {
			Runtime::PlaySound("GTSSoundFail", pred, 0.4f, 1.0f);
		}
		auto& voreData = this->GetVoreData(pred);
		voreData.AddTiny(prey);

		AnimationManager::StartAnim("StartVore", pred);
	}

	void VoreController::RecordOriginalScale(Actor* tiny) {
		auto Data = Transient::GetSingleton().GetData(tiny);
		if (Data) {
			Data->VoreRecordedScale = std::clamp(get_visual_scale(tiny), 0.02f, 1000000.0f);
		}
	}

	float VoreController::ReadOriginalScale(Actor* tiny) {
		auto Data = Transient::GetSingleton().GetData(tiny);
		if (Data) {
			return Data->VoreRecordedScale;
		}
		return 1.0f;
	}

	void VoreController::ShrinkOverTime(Actor* giant, Actor* tiny, float time_mult, float targetscale_mult) {
		if (tiny) {
			float Adjustment_Tiny = GetSizeFromBoundingBox(tiny);
			float preyscale = get_visual_scale(tiny) * Adjustment_Tiny;
			float targetScale = std::clamp(preyscale/12.0f * Adjustment_Tiny, 0.01f, 1000000.0f);
			targetScale *= targetscale_mult;

			float shrink_magnitude = -targetScale * time_mult;

			ActorHandle tinyHandle = tiny->CreateRefHandle();

			std::string name = std::format("ShrinkTo_{}", tiny->formID);

			if (preyscale > targetScale) {
				VoreController::GetSingleton().RecordOriginalScale(tiny); // We're shrinking the tiny which affects effectiveness of vore bonuses, this fixes it
				TaskManager::Run(name, [=](auto& progressData) {
					Actor* actor = tinyHandle.get().get();
					if (!actor) {
						return false;
					}

					float scale = get_visual_scale(actor);

					if (scale > targetScale) {
						override_actor_scale(actor, shrink_magnitude * 0.225f * TimeScale(), SizeEffectType::kNeutral);
						if (get_target_scale(actor) < targetScale) {
							set_target_scale(actor, targetScale);
							return false;
						}
						return true;
					} else {
						return false;
					}

					return false;
				});
			}
		}
	}

	// Gets the current vore data of a giant
	VoreData& VoreController::GetVoreData(Actor* giant) {
		std::unique_lock lock(_lock);
		// Create it now if not there yet
		this->data.try_emplace(giant->formID, giant);

		return this->data.at(giant->formID);
	}

	void VoreController::AllowMessage(bool allow) {
		this->allow_message = allow;
	}
}
