#include "Managers/Animation/Controllers/ButtCrushController.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Managers/Animation/Utils/AttachPoint.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/BoobCrush.hpp"
#include "Managers/GTSSizeManager.hpp"
#include "Managers/Rumble.hpp"
#include "Managers/HighHeel.hpp"
#include "Utils/Actions/ButtCrushUtils.hpp"

using namespace GTS;

namespace {

	constexpr float MINIMUM_BUTTCRUSH_DISTANCE = 95.0f;
	constexpr float BUTTCRUSH_ANGLE = 70;
	constexpr float PI = std::numbers::pi_v<float>;

	void AttachToObjectBTask(Actor* giant, Actor* tiny) {
		std::string name = std::format("ButtCrush_{}", tiny->formID);
		SetBeingEaten(tiny, true);

		if (IsCrawling(giant)) {
			AnimationBoobCrush::GetSingleton().AttachActor(giant, tiny);
		}

		auto gianthandle = giant->CreateRefHandle();
		auto tinyhandle = tiny->CreateRefHandle();
		
		auto FrameA = Time::FramesElapsed();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			if (!tinyhandle) {
				return false;
			}
			auto FrameB = Time::FramesElapsed() - FrameA;
			if (FrameB <= 10.0f) {
				return true;
			}
			auto giantref = gianthandle.get().get();
			auto tinyref = tinyhandle.get().get();

			auto node = find_node(giantref, "AnimObjectB");
			if (!node) {
				return false;
			}

			if (!IsActionOnCooldown(giantref, CooldownSource::Misc_ShrinkParticle_Animation)) {
				auto node_tiny = find_node(tinyref, "NPC Root [Root]");
				if (node_tiny) {
					float rune_scale = get_visual_scale(tinyref) * GetSizeFromBoundingBox(tinyref);
					SpawnParticle(tinyref, 3.00f, "GTS/gts_tinyrune.nif", NiMatrix3(), node_tiny->world.translate, rune_scale, 7, node_tiny); 
				}
				ApplyActionCooldown(giantref, CooldownSource::Misc_ShrinkParticle_Animation);
			}

			ForceRagdoll(tinyref, false);
			float stamina = GetAV(giantref, ActorValue::kStamina);
			float Difference = std::clamp(GetSizeDifference(giantref, tinyref, SizeType::VisualScale, true, false), 1.0f, 10.0f);

			
			DamageAV(giantref, ActorValue::kStamina, 0.04f * GetButtCrushCost(giantref, false));
			DamageAV(giantref, ActorValue::kMagicka, 1.4f * (GetButtCrushCost(giantref, true)) / Difference);

			ApplyActionCooldown(giantref, CooldownSource::Action_ButtCrush); // Set butt crush on the cooldown

			if (stamina <= 2.0f && !IsChangingSize(giantref)) {
				AnimationManager::StartAnim("ButtCrush_Attack", giantref); // Try to Abort it
			}

			if (GetAV(giantref, ActorValue::kHealth) <= 1.0f || giantref->IsDead()) {
				SetButtCrushSize(giantref, 0.0f, true);
				PushActorAway(giantref, giantref, 1.0f);
				PushActorAway(tinyref, tinyref, 1.0f);

				SetBeingEaten(tinyref, false);
				EnableCollisions(tiny);

				SpawnCustomParticle(giantref, ParticleType::Red, NiPoint3(), "NPC Root [Root]", 3.0f);
				SpawnParticle(giantref, 4.60f, "GTS/Effects/TinyCalamity.nif", NiMatrix3(), giantref->GetPosition(), get_visual_scale(giantref) * 4.0f, 7, nullptr);
				Runtime::PlaySoundAtNode_FallOff(Runtime::SNDR.GTSSoundTinyCalamity_Impact, giantref, 1.0f, "NPC COM [COM ]", 0.10f * get_visual_scale(giantref));
				Rumbling::Once("ButtCrushDeath", giantref, 128.0f, 0.25f, "NPC Root [Root]", 0.0f);

				AnimationBoobCrush::GetSingleton().Reset();

				return false;
			}

			auto coords = node->world.translate;
			if (!IsCrawling(giantref)) {
				float HH = HighHeelManager::GetHHOffset(giantref).Length();
				coords.z -= HH;
			} 
			if (!IsButtCrushing(giantref)) {
				AnimationBoobCrush::GetSingleton().Reset();
				SetBeingEaten(tinyref, false);
				EnableCollisions(tinyref);
				return false;
			}
			if (!AttachTo_NoForceRagdoll(giantref, tinyref, coords)) {
				AnimationBoobCrush::GetSingleton().Reset();
				SetBeingEaten(tinyref, false);
				EnableCollisions(tinyref);
				return false;
			}
			if (tinyref->IsDead()) {
				AnimationBoobCrush::GetSingleton().Reset();
				SetBeingEaten(tinyref, false);
				EnableCollisions(tinyref);
				return false;
			}
			return true;
		});
	}

	void CantButtCrushPlayerMessage(Actor* giant, Actor* tiny, float sizedifference) {
		if (sizedifference < Action_ButtCrush) {
			std::string message = std::format("Player is too big for Butt Crush: x{:.2f}/{:.2f}", sizedifference, Action_ButtCrush);
			NotifyWithSound(tiny, message);
		}
	}
}

namespace GTS {

	std::string ButtCrushController::DebugName() {
		return "::ButtCrushController";
	}

	void ButtCrushController::ButtCrush_OnCooldownMessage(Actor* giant) {
		double cooldown = GetRemainingCooldown(giant, CooldownSource::Action_ButtCrush);
		std::string message;
		if (giant->formID == 0x14) {
			if (!IsCrawling(giant) && !giant->IsSneaking()) {
				message = std::format("Butt Crush is on a cooldown: {:.1f} sec", cooldown);
			} else if (giant->IsSneaking() && !IsCrawling(giant)) {
				message = std::format("Knee Crush is on a cooldown: {:.1f} sec", cooldown);
			} else {
				message = std::format("Breast Crush is on a cooldown: {:.1f} sec", cooldown);
			}
			shake_camera(giant, 0.45f, 0.30f);
			NotifyWithSound(giant, message);
		} else if (IsTeammate(giant) && !IsGtsBusy(giant)) {
			if (!IsCrawling(giant) && !giant->IsSneaking()) {
				message = std::format("Follower's Butt Crush is on a cooldown: {:.1f} sec", cooldown);
			} else if (giant->IsSneaking() && !IsCrawling(giant)) {
				message = std::format("Follower's Knee Crush is on a cooldown: {:.1f} sec", cooldown);
			} else {
				message = std::format("Follower's Breast Crush is on a cooldown: {:.1f} sec", cooldown);
			}
			NotifyWithSound(giant, message);
		}
	}

	std::vector<Actor*> ButtCrushController::GetButtCrushTargets(Actor* pred, std::size_t numberOfPrey) {
		// Get vore target for actor
		auto& sizemanager = SizeManager::GetSingleton();
		if (!CanPerformAnimation(pred, AnimationCondition::kGrabAndSandwich)) {
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
		ranges::sort(preys,[predPos](const Actor* preyA, const Actor* preyB) -> bool {
			float distanceToA = (preyA->GetPosition() - predPos).Length();
			float distanceToB = (preyB->GetPosition() - predPos).Length();
			return distanceToA < distanceToB;
		});

		// Filter out invalid targets
		std::erase_if(preys,[pred, this](auto prey){
			return !this->CanButtCrush(pred, prey);
		});

		// Filter out actors not in front
		auto actorAngle = pred->data.angle.z;
		RE::NiPoint3 forwardVector{ 0.f, 1.f, 0.f };
		RE::NiPoint3 actorForward = RotateAngleAxis(forwardVector, -actorAngle, { 0.f, 0.f, 1.f });

		NiPoint3 predDir = actorForward;
		predDir = predDir / predDir.Length();
		std::erase_if(preys,[predPos, predDir](auto prey) {
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
		float shiftAmount = fabs((predWidth / 2.0f) / tan(BUTTCRUSH_ANGLE/2.0f));

		NiPoint3 coneStart = predPos - predDir * shiftAmount;
		preys.erase(std::remove_if(preys.begin(), preys.end(),[coneStart, predWidth, predDir](auto prey)
		{
			NiPoint3 preyDir = prey->GetPosition() - coneStart;
			if (preyDir.Length() <= predWidth*0.4f) {
				return false;
			}
			preyDir = preyDir / preyDir.Length();
			float cosTheta = predDir.Dot(preyDir);
			return cosTheta <= cos(BUTTCRUSH_ANGLE*PI/180.0f);
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

	bool ButtCrushController::CanButtCrush(Actor* pred, Actor* prey) const {
		if (pred == prey) {
			return false;
		}

		if (prey->IsDead()) {
			return false;
		}

		float pred_scale = get_visual_scale(pred);
		float sizedifference = GetSizeDifference(pred, prey, SizeType::VisualScale, true, false);

		float MINIMUM_BUTTCRUSH_SCALE = Action_ButtCrush;
		float MINIMUM_DISTANCE = MINIMUM_BUTTCRUSH_DISTANCE;
		if (IsCrawling(pred)) {
			MINIMUM_BUTTCRUSH_SCALE *= 1.5f;
		}

		float prey_distance = (pred->GetPosition() - prey->GetPosition()).Length();
		if (prey_distance <= MINIMUM_DISTANCE * pred_scale && sizedifference < MINIMUM_BUTTCRUSH_SCALE) {
			if (pred->formID == 0x14) {
				std::string_view message = fmt::format("{} is too big for Butt Crush: x{:.2f}/{:.2f}", prey->GetDisplayFullName(), sizedifference, MINIMUM_BUTTCRUSH_SCALE);
				if (!IsCrawling(pred) && pred->IsSneaking()) {
					message = fmt::format("{} is too big for Knee Crush: x{:.2f}/{:.2f}", prey->GetDisplayFullName(), sizedifference, MINIMUM_BUTTCRUSH_SCALE);
				} else if (IsCrawling(pred)) {
					message = fmt::format("{} is too big for Breast Crush: x{:.2f}/{:.2f}", prey->GetDisplayFullName(), sizedifference, MINIMUM_BUTTCRUSH_SCALE);
				} 
				shake_camera(pred, 0.45f, 0.30f);
				NotifyWithSound(pred, message);
			} else if (this->allow_message && prey->formID == 0x14 && IsTeammate(pred)) {
				CantButtCrushPlayerMessage(pred, prey, sizedifference);
			}
			return false;
		}
		if (prey_distance <= (MINIMUM_DISTANCE * pred_scale) && sizedifference >= MINIMUM_BUTTCRUSH_SCALE) {
			if (IsFlying(prey)) {
				return false; // Disallow to butt crush flying dragons
			}
			if ((prey->formID != 0x14 && !CanPerformAnimationOn(pred, prey, false))) {
				std::string_view message = std::format("{} is Essential", prey->GetDisplayFullName());
				NotifyWithSound(pred, message);
				return false;
			}
			return true;
		} else {
			return false;
		}
	}

	void ButtCrushController::StartButtCrush(Actor* pred, Actor* prey, const bool dochecks) {
		auto& buttcrush = ButtCrushController::GetSingleton();

		if (dochecks) {
			if (!buttcrush.CanButtCrush(pred, prey)) {
				return;
			}
		}

		if (CanDoButtCrush(pred, false) && !IsBeingHeld(pred, prey)) {
			prey->NotifyAnimationGraph("GTS_EnterFear");
			
			if (GetSizeDifference(pred, prey, SizeType::VisualScale, false, false) < Action_ButtCrush) {
				ShrinkUntil(pred, prey, 3.4f, 0.25f, true);
				return;
			}

			DisableCollisions(prey, pred);

			float WasteStamina = 60.0f * GetButtCrushCost(pred, false);

			AttachToObjectBTask(pred, prey);
			ApplyActionCooldown(pred, CooldownSource::Action_ButtCrush); // Set butt crush on the cooldown

			ActorHandle giantHandle = pred->CreateRefHandle();
			std::string taskname = std::format("ButtCrushMagicka_{}", pred->formID);
			TaskManager::RunOnce(taskname, [=](auto& update){

				if (!giantHandle) {
					return;
				}

				auto giantref = giantHandle.get().get();
				DamageAV(giantref, ActorValue::kMagicka, 180 * GetButtCrushCost(giantref, true));
				DamageAV(giantref, ActorValue::kStamina, WasteStamina);
			});

			AnimationManager::StartAnim("ButtCrush_Start", pred);
		}
		else {
			ButtCrush_OnCooldownMessage(pred);
		}
	}

	void ButtCrushController::AllowMessage(bool allow) {
		this->allow_message = allow;
	}
}