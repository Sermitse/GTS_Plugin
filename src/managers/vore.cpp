#include "managers/animation/AnimationManager.hpp"
#include "managers/animation/Utils/AnimationUtils.hpp"
#include "managers/ai/aifunctions.hpp"
#include "managers/GtsSizeManager.hpp"
#include "managers/InputManager.hpp"
#include "magic/effects/common.hpp"
#include "managers/perks/PerkHandler.hpp"
#include "utils/SurvivalMode.hpp"
#include "utils/actorUtils.hpp"
#include "managers/Rumble.hpp"
#include "data/persistent.hpp"
#include "data/transient.hpp"
#include "scale/modscale.hpp"
#include "ActionSettings.hpp"
#include "utils/looting.hpp"
#include "managers/vore.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"
#include "profiler.hpp"
#include "timer.hpp"
#include "node.hpp"
#include <cmath>
#include <random>

using namespace RE;
using namespace Gts;

namespace {
	const float MINIMUM_VORE_DISTANCE = 94.0f;
	const float VORE_ANGLE = 76;
	const float PI = 3.14159f;

	void CantVorePlayerMessage(Actor* giant, Actor* tiny, float sizedifference) {
		if (sizedifference < Action_Vore) {
			std::string message = std::format("Player is too big to be eaten: x{:.2f}/{:.2f}", sizedifference, Action_Vore);
			NotifyWithSound(tiny, message);
		}
	}

	VoreInformation GetVoreInfo(Actor* giant, Actor* tiny, float growth_mult) {
		float recorded_scale = Vore::GetSingleton().ReadOriginalScale(tiny);
		float restore_power = 0.0f;
		float mealEfficiency = 0.2f; // Normal pred has 20% efficent stomach
		float duration = 80.0f; // 80 seconds duration by default
		float growth = 2.0f; // Default power of gaining size

		std::string_view tiny_name = tiny->GetDisplayFullName();

		if (Runtime::HasPerkTeam(giant, "VorePerk")) {
			restore_power = GetMaxAV(tiny, ActorValue::kHealth) * mealEfficiency; // Default hp/sp regen
		}

		if (Runtime::HasPerkTeam(giant, "Gluttony")) {
			restore_power = GetMaxAV(tiny, ActorValue::kHealth) * 4 * mealEfficiency; // 4 times stronger hp/sp regen
			mealEfficiency += 0.2f; // 100% more growth
			duration *= 0.5f; // 50% faster vore
		}
		if (Runtime::HasPerkTeam(giant, "AdditionalGrowth")) {
			growth *= 1.25f; // 25% stronger growth
		}
			
		float bounding_box = GetSizeFromBoundingBox(tiny);
		float gain_power = recorded_scale * mealEfficiency * growth * growth_mult * bounding_box; // power of most buffs that we start
		// ^ power of gaining size, it is further reduced by player size during OnUpdate() func below

		VoreInformation VoreInfo = VoreInformation { // Create Vore Info
			.giantess = giant,
			.WasLiving = IsLiving(tiny),
			.Scale = recorded_scale,
			.Vore_Power = gain_power,
			.Restore_Power = restore_power,
			.Natural_Scale = bounding_box,
			.Duration = duration,
			.Tiny_Name = tiny->GetDisplayFullName(),
		};

		return VoreInfo;
	}

	void BuffAttributes(Actor* giant, float tinyscale) {
		if (giant) {
			if (Runtime::HasPerk(giant, "SoulVorePerk")) { // Permamently increases random AV after eating someone
				float TotalMod = 0.33f;
				int Boost = RandomInt(0, 3);
				if (Boost == 0) {
					AddStolenAttributesTowards(giant, ActorValue::kHealth, TotalMod);
				} else if (Boost == 1) {
					AddStolenAttributesTowards(giant, ActorValue::kStamina, TotalMod);
				} else if (Boost >= 2) {
					AddStolenAttributesTowards(giant, ActorValue::kMagicka, TotalMod);
				}
			}
		}
	}

	void VoreMessage_SwallowedAbsorbing(Actor* pred, Actor* prey) {
		if (pred) {
			int random = RandomInt(0, 3);
			if (!prey->IsDead() && !Runtime::HasPerk(pred, "SoulVorePerk") || random <= 1) {
				Cprint("{} was Swallowed and is now being slowly absorbed by {}", prey->GetDisplayFullName(), pred->GetDisplayFullName());
			} else if (random == 2) {
				Cprint("{} is now absorbing {}", pred->GetDisplayFullName(), prey->GetDisplayFullName());
			} else if (random >= 3) {
				Cprint("{} will soon be completely absorbed by {}", prey->GetDisplayFullName(), pred->GetDisplayFullName());
			}
		}
	}

	void VoreMessage_Absorbed(Actor* pred, std::string_view prey) {
		if (pred) {
			int random = RandomInt(0, 3);
			if (!Runtime::HasPerk(pred, "SoulVorePerk") || random == 0) {
				Cprint("{} was absorbed by {}", prey, pred->GetDisplayFullName());
			} else if (Runtime::HasPerk(pred, "SoulVorePerk") && random == 1) {
				Cprint("{} became one with {}", prey, pred->GetDisplayFullName());
			} else if (Runtime::HasPerk(pred, "SoulVorePerk") && random >= 2) {
				Cprint("{} was greedily devoured by {}", prey, pred->GetDisplayFullName());
			} else {
				Cprint("{} was absorbed by {}", prey, pred->GetDisplayFullName());
			}
		}
	}

	void Vore_AdvanceQuest(Actor* pred, Actor* tiny, bool WasDragon, bool WasGiant) {
		if (pred->formID == 0x14 && WasDragon) {
			CompleteDragonQuest(tiny, ParticleType::Blue, false);
			return;
		}
		if (WasGiant) {
			AdvanceQuestProgression(pred, tiny, QuestStage::Giant, 1, true);
		} else {
			AdvanceQuestProgression(pred, tiny, QuestStage::Vore, 1, true);
		}
	}

	void Task_Vore_FinishVoreBuff(const VoreInformation& VoreInfo, int amount_of_tinies, bool Allow_Devourment) {

		Actor* giant = VoreInfo.giantess;

		bool WasLiving = VoreInfo.WasLiving;

		float tinySize = VoreInfo.Scale;
		float sizePower = VoreInfo.Vore_Power;
		float natural_scale = VoreInfo.Natural_Scale;

		std::string_view tiny_name = VoreInfo.Tiny_Name;

		bool Devourment = AllowDevourment();
		float multiplier = 1.0f;
		if (Devourment) {
			multiplier = 0.5f;
		}

		if (!Devourment || Allow_Devourment) {
			if (giant) {
				float tinySize_Grow = std::clamp(tinySize, 1.0f, 10000.0f);
				float reduction = std::clamp(get_visual_scale(giant) / tinySize_Grow, 1.0f, 10.0f);
				update_target_scale(giant, (sizePower/reduction) * 0.52f, SizeEffectType::kGrow);
				GainWeight(giant, 3.0f * tinySize * amount_of_tinies * multiplier);
				ModSizeExperience(giant, 0.20f * multiplier + (tinySize * 0.02f));
				VoreMessage_Absorbed(giant, tiny_name);
				AdjustSizeReserve(giant, sizePower);
				BuffAttributes(giant, tinySize);
				
				if (giant->formID == 0x14) {
					AdjustSizeLimit(0.0260f * multiplier, giant);
					AdjustMassLimit(0.0106f * multiplier, giant);
					SurvivalMode_AdjustHunger(giant, tinySize * natural_scale * multiplier, WasLiving, true);
				}
				Rumbling::Once("GrowthRumble", giant, 1.75f, 0.30f);
				if (Vore::GetSingleton().GetVoreData(giant).GetTimer() == true) {
					PlayMoanSound(giant, 1.0f); // play timed sound. Timer is a must else we moan 10 times at once for example.
					Task_FacialEmotionTask_Moan(giant, 2.0f, "Vore");
				}
			}
		}
	}

	void Task_Vore_StartVoreBuff(Actor* giant, Actor* tiny, int amount_of_tinies) {
		if (!AllowDevourment()) {
			std::string name = std::format("Vore_Buff_{}_{}", giant->formID, tiny->formID);
			VoreInformation VoreInfo = GetVoreInfo(giant, tiny, 1.0f);
			ActorHandle gianthandle = giant->CreateRefHandle();
			double start_time = Time::WorldTimeElapsed();

			float Regeneration = VoreInfo.Restore_Power;
			float Recorded_Scale = VoreInfo.Scale;
			float Growth = VoreInfo.Vore_Power;
			float Duration = VoreInfo.Duration;

			float tinySize_Grow = std::clamp(Recorded_Scale, 1.0f, 10000.0f);

			TaskManager::Run(name, [=](auto& progressData) {
				if (!gianthandle) {
					return false;
				}
				auto giantref = gianthandle.get().get();
				double timepassed = Time::WorldTimeElapsed() - start_time;

				float reduction = std::clamp(get_visual_scale(giantref) / tinySize_Grow, 1.0f, 10.0f);
				float regen_attributes = GetMaxAV(giantref, ActorValue::kHealth) * 0.0006f;
				float health = std::clamp(Regeneration/4000.0f, 0.0f, regen_attributes);

				float sizeToApply = Growth/5000;

				DamageAV(giantref, ActorValue::kHealth, -health * TimeScale());
				DamageAV(giantref, ActorValue::kStamina, -health * TimeScale()); 
				// Restore HP and Stamina for GTS

				if (get_target_scale(giantref) < get_max_scale(giantref)) { // For some reason likes to surpass size limit by ~0.03 (multiplies by race scale)
					update_target_scale(giantref, (sizeToApply / reduction) * TimeScale(), SizeEffectType::kGrow);
					AddStolenAttributes(giantref, sizeToApply * TimeScale());
				}

				if (timepassed >= Duration) {
					Task_Vore_FinishVoreBuff(VoreInfo, amount_of_tinies, false);
					if (giantref->formID == 0x14) {
						shake_camera(giantref, 0.50f, 0.75f);
					}
					return false;
				}

				return true;
			});
		}
	}

	void DevourmentBonuses(Actor* Pred, Actor* Prey, bool Digested, float mult) {
		VoreInformation VoreInfo = GetVoreInfo(Pred, Prey, mult);

		if (Digested) {
			Vore_AdvanceQuest(Pred, Prey, IsDragon(Prey), IsGiant(Prey)); // Progress quest
			if (Pred->formID == 0x14) {
				shake_camera(Pred, 0.50f, 0.75f);
			}
		}

		Task_Vore_FinishVoreBuff(VoreInfo, 1, true);
	}
}

namespace Gts {
	VoreData::VoreData(Actor* giant) : giant(giant? giant->CreateRefHandle() : ActorHandle()) {
	}

	void VoreData::AddTiny(Actor* tiny) {
		this->tinies.try_emplace(tiny->formID, tiny->CreateRefHandle());
	}

	void VoreData::EnableMouthShrinkZone(bool enabled) {
		this->killZoneEnabled = enabled;
	}

	void VoreData::Swallow() {
		for (auto& [key, tinyref]: this->tinies) {
			auto tiny = tinyref.get().get();
			auto giant = this->giant.get().get();
			
			if (giant->formID == 0x14) {
				if (IsLiving(tiny) && IsHuman(tiny)) {
					CallVampire();
				}
				bool Giant = IsGiant(tiny);
				bool Living = IsLiving(tiny);
				bool Dragon = IsDragon(tiny);
				bool Mammoth = IsMammoth(tiny);
				
				float DefaultScale = get_natural_scale(tiny);
				ModSizeExperience(giant, 0.08f + (DefaultScale*0.025f));

				SurvivalMode_AdjustHunger(this->giant.get().get(), Vore::GetSingleton().ReadOriginalScale(tiny) * GetSizeFromBoundingBox(tiny), Living, false);
			}

			Task_Vore_StartVoreBuff(giant, tiny, static_cast<int>(this->tinies.size()));
			VoreMessage_SwallowedAbsorbing(giant, tiny);
		}
	}
	void VoreData::KillAll() {
		if (!AllowDevourment()) {
			for (auto& [key, tinyref]: this->tinies) {
				auto tiny = tinyref.get().get();
				auto giantref = this->giant;
				SetBeingHeld(tiny, false);
				AddSMTDuration(giantref.get().get(), 6.0f);
				if (tiny->formID != 0x14) {
					KillActor(giantref.get().get(), tiny);
					PerkHandler::UpdatePerkValues(giantref.get().get(), PerkUpdate::Perk_LifeForceAbsorption);
				} else if (tiny->formID == 0x14) {
					InflictSizeDamage(giantref.get().get(), tiny, 900000);
					KillActor(giantref.get().get(), tiny);
					TriggerScreenBlood(50);
					tiny->SetAlpha(0.0f); // Player can't be disintegrated: simply nothing happens. So we Just make player Invisible instead.
				}

				Vore_AdvanceQuest(giantref.get().get(), tiny, IsDragon(tiny), IsGiant(tiny)); // Progress quest

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
		for (auto& [key, tinyref]: this->tinies) {
			auto tiny = tinyref.get().get();
			auto transient = Transient::GetSingleton().GetData(tiny);
			if (transient) {
				transient->can_be_vored = allow;
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
		std::vector<Actor*> result;
		for (auto& [key, actorref]: this->tinies) {
			auto actor = actorref.get().get();
			result.push_back(actor);
		}
		return result;
	}

	void VoreData::Update() {
		auto profiler = Profilers::Profile("Vore: Update");
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

	Vore& Vore::GetSingleton() noexcept {
		static Vore instance;
		return instance;
	}

	std::string Vore::DebugName() {
		return "Vore";
	}

	void Vore::Update() {
		for (auto& [key, voreData]: this->data) {
			voreData.Update();
		}
	}

	Actor* Vore::GeVoreTargetCrossHair(Actor* pred) {
		auto preys = this->GeVoreTargetsCrossHair(pred, 1);
		if (preys.size() > 0) {
			return preys[0];
		} else {
			return nullptr;
		}
	}

	std::vector<Actor*> Vore::GeVoreTargetsCrossHair(Actor* pred, std::size_t numberOfPrey) {
		// Get vore target for player
		if (!pred) {
			return {};
		}
		auto playerCamera = PlayerCamera::GetSingleton();
		if (!playerCamera) {
			return {};
		}
		auto crosshairPick = RE::CrosshairPickData::GetSingleton();
		if (!crosshairPick) {
			return {};
		}
		auto cameraNode = playerCamera->cameraRoot.get();
		if (!cameraNode) {
			return {};
		}
		NiPoint3 start = cameraNode->world.translate;
		NiPoint3 end = crosshairPick->collisionPoint;

		auto preys = find_actors();
		auto predPos = pred->GetPosition();

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
		}), preys.end());;

		// Filter out actors not in front
		NiPoint3 predDir = end - start;
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

		NiPoint3 coneStart = start;
		preys.erase(std::remove_if(preys.begin(), preys.end(),[coneStart, predDir](auto prey)
		{
			NiPoint3 preyDir = prey->GetPosition() - coneStart;
			if (preyDir.Length() <= 1e-4) {
				return false;
			}
			preyDir = preyDir / preyDir.Length();
			float cosTheta = predDir.Dot(preyDir);
			return cosTheta <= cos(VORE_ANGLE*PI/180.0f);
		}), preys.end());

		if (numberOfPrey == 1) {
			return Vore_GetMaxVoreCount(pred, preys);
		}

		// Reduce vector size
		if (preys.size() > numberOfPrey) {
			preys.resize(numberOfPrey);
		}

		return preys;
	}

	Actor* Vore::GetVoreTargetInFront(Actor* pred) {
		auto preys = this->GetVoreTargetsInFront(pred, 1);
		if (preys.size() > 0) {
			return preys[0];
		} else {
			return nullptr;
		}
	}

	std::vector<Actor*> Vore::GetVoreTargetsInFront(Actor* pred, std::size_t numberOfPrey) {
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
		}), preys.end());;

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
			return Vore_GetMaxVoreCount(pred, preys);
		}

		// Reduce vector size
		if (preys.size() > numberOfPrey) {
			preys.resize(numberOfPrey);
		}

		return preys;
	}

	Actor* Vore::GetVoreTargetAround(Actor* pred) {
		auto preys = this->GetVoreTargetsAround(pred, 1);
		if (preys.size() > 0) {
			return preys[0];
		} else {
			return nullptr;
		}
	}

	std::vector<Actor*> Vore::GetVoreTargetsAround(Actor* pred, std::size_t numberOfPrey) {
		// Get vore target for actor
		// around them
		if (!pred) {
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

		if (numberOfPrey == 1) {
			return Vore_GetMaxVoreCount(pred, preys);
		}

		// Reduce vector size
		if (preys.size() > numberOfPrey) {
			preys.resize(numberOfPrey);
		}

		return preys;
	}


	bool Vore::CanVore(Actor* pred, Actor* prey) {
		if (pred == prey) {
			return false;
		}
		if (!CanPerformAnimation(pred, 3)) {
			return false;
		}

		if (prey->formID == 0x14 && !Persistent::GetSingleton().vore_allowplayervore) {
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
			if (transient->can_be_vored == false) {
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

		if (IsInsect(prey, true) || IsBlacklisted(prey) || IsUndead(prey, true)) {
			std::string_view message = fmt::format("{} has no desire to eat {}", pred->GetDisplayFullName(), prey->GetDisplayFullName());
			NotifyWithSound(pred, message);
			return false;
		}

		if (prey_distance <= (MINIMUM_DISTANCE * pred_scale) && sizedifference < MINIMUM_VORE_SCALE) {
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
			} else {
				return true;
			}
		} else {
			return false;
		}
	}

	void Vore::Reset() {
		this->data.clear();
	}

	void Vore::ResetActor(Actor* actor) {
		this->data.erase(actor->formID);
	}

	void Vore::StartVore(Actor* pred, Actor* prey) {
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

		if (!Runtime::HasPerkTeam(pred, "VorePerk")) { // Damage stamina if we don't have perk
			if (staminacheck < wastestamina) {
				Notify("{} is too tired for vore.", pred->GetDisplayFullName());
				DamageAV(prey, ActorValue::kHealth, 3 * sizedifference);
				if (pred->formID == 0x14) {
					Runtime::PlaySound("VoreSound_Fail", pred, 0.4f, 1.0f);
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
			Runtime::PlaySound("VoreSound_Fail", pred, 0.4f, 1.0f);
		}
		auto& voreData = this->GetVoreData(pred);
		voreData.AddTiny(prey);

		AnimationManager::GetSingleton().StartAnim("StartVore", pred);
	}

	void Vore::RecordOriginalScale(Actor* tiny) {
		auto Data = Transient::GetSingleton().GetData(tiny);
		if (Data) {
			Data->vore_recorded_scale = std::clamp(get_visual_scale(tiny), 0.02f, 999999.0f);
		}
	}

	float Vore::ReadOriginalScale(Actor* tiny) {
		auto Data = Transient::GetSingleton().GetData(tiny);
		if (Data) {
			return Data->vore_recorded_scale;
		}
		return 1.0f;
	}

	void Vore::ShrinkOverTime(Actor* giant, Actor* tiny, float over_time) {
		if (tiny) {
			float Adjustment_Tiny = GetSizeFromBoundingBox(tiny);
			float preyscale = get_visual_scale(tiny) * Adjustment_Tiny;
			float targetScale = std::clamp(preyscale/12.0f * Adjustment_Tiny, 0.01f, 999999.0f);

			float shrink_magnitude = -targetScale;

			ActorHandle tinyHandle = tiny->CreateRefHandle();

			std::string name = std::format("ShrinkTo_{}", tiny->formID);

			if (preyscale > targetScale) {
				Vore::GetSingleton().RecordOriginalScale(tiny); // We're shrinking the tiny which affects effectiveness of vore bonuses, this fixes it
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

	void Vore::Devourment_Compatibility(Actor* Pred, Actor* Prey, bool Digested) {
		if (Pred) {
			if (Prey) {
				auto Data = Transient::GetSingleton().GetData(Prey);
				if (Data) {
					bool& Devoured = Data->Devourment_Devoured;
					bool& Eaten = Data->Devourment_Eaten;
					if (Digested && !Devoured) { // Actors was completely absorbed (stage 2)
						Notify("{} was devoured by {}", Prey->GetDisplayFullName(), Pred->GetDisplayFullName());
						Cprint("{} was devoured by {}", Prey->GetDisplayFullName(), Pred->GetDisplayFullName());
						DevourmentBonuses(Pred, Prey, true, 1.0f);
						Devoured = true;
					} else if (!Eaten) { // health bar was absorbed (stage 1)
						DevourmentBonuses(Pred, Prey, false, 1.0f);
						log::info("Adding to Eaten faction");
						Eaten = true;
					}
				}
			}
		}
	}

	// Gets the current vore data of a giant
	VoreData& Vore::GetVoreData(Actor* giant) {
		// Create it now if not there yet
		this->data.try_emplace(giant->formID, giant);

		return this->data.at(giant->formID);
	}

	void Vore::AllowMessage(bool allow) {
		this->allow_message = allow;
	}
}
