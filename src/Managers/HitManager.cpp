#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Managers/GTSSizeManager.hpp"
#include "Managers/HitManager.hpp"

using namespace GTS;

namespace {

	float GetPushPower(float sizeRatio) {
		// https://www.desmos.com/calculator/wh0vwgljfl
		SoftPotential push {
			.k = 1.42f,
			.n = 0.78f,
			.s = 0.50f,
			.a = 0.0f,
		};
		float power = soft_power(sizeRatio, push);
		return power;
	}

	const bool TinyCalamity_PushFromHit(Actor* attacker, Actor* receiver, bool wasPowerAttack, std::string_view hitName) {
		if (TinyCalamityActive(attacker)) {
			if (hitName.contains("Bow")) {
				return false;
			}
			float size_difference = get_scale_difference(attacker, receiver, SizeType::VisualScale, true, false);
			float camShake = wasPowerAttack ? size_difference * 0.125f : size_difference * 0.075f;
			float soundVolume = wasPowerAttack ? 0.5f : 0.3f;
			float penalty = wasPowerAttack ? 0.5f : 0.25f;

			const float ragdollThreshold = wasPowerAttack ? 0.35 : 0.25f;
			float hp = GetHealthPercentage(receiver);

			const bool canRagdoll = hp <= 0.25f * (wasPowerAttack ? 0.75f : 1.0f) && !IsActionOnCooldown(receiver, CooldownSource::Misc_TinyCalamity_Ragdoll);
			const bool canStagger = !IsActionOnCooldown(receiver, CooldownSource::Misc_TinyCalamity_Hit);	

			float staggerChance = std::clamp(1.35f - hp, 0.35f, 1.0f);
			const bool allowStagger = RandomFloat(0.0f, 100.0f) <= staggerChance * 100.0f;
			logger::info("HP %: {}, can Ragdoll: {}", hp, canRagdoll);

			if (canRagdoll) {
				SpawnParticle(receiver, 6.00f, "GTS/Effects/TinyCalamity.nif", NiMatrix3(), receiver->GetPosition(), get_visual_scale(receiver) * 4.5f, 7, nullptr); 
				PushForward(attacker, receiver, std::clamp(160.0f * size_difference, 120.0f, 1200.0f));
				ApplyActionCooldown(receiver, CooldownSource::Misc_TinyCalamity_Ragdoll);
				soundVolume = 1.0f;
				camShake *= 2.0f;
				penalty += 0.5f;
			} else if (canStagger) {
				const float pushPower = std::clamp(0.25f + (1.0f - hp), 0.25f, 1.0f);
				ApplyActionCooldown(receiver, CooldownSource::Misc_TinyCalamity_Hit);
				StaggerActor(attacker, receiver, pushPower);
				penalty += 0.25f;
			} 
			Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundTinyCalamity_Impact, receiver, soundVolume, "NPC COM [COM ]");
			shake_camera(attacker, camShake, 0.35f);
			AddSMTPenalty(attacker, penalty);
			return true;
		}
		return false;
	}
}

namespace GTS {

	std::string HitManager::DebugName() {
		return "::HitManager";
	}

	void HitManager::HitEvent(const TESHitEvent* a_event) {
		if (!a_event) {
			return;
		}
		auto attacker_PTR = a_event->cause;
		auto atacker_ref = attacker_PTR.get();
		auto attacker = skyrim_cast<Actor*>(atacker_ref);
		if (!attacker) {
			return;
		}
		auto receiver_PTR = a_event->target;
		auto receiver_ref = receiver_PTR.get();
		auto receiver = skyrim_cast<Actor*>(receiver_ref);
		if (!receiver) {
			return;
		}

		auto HitIdForm = a_event->source;
		auto HitId = TESForm::LookupByID(HitIdForm);

		std::string hitName = HitId->GetName();

		if (hitName == "Stagger" || hitName == "SizeEffect" || hitName == "SprintingSizeEffect" || hitName == "GtsTastyFoe") {
			return;
		}
		auto ProjectileIDForm = a_event->projectile;
		auto ProjectileID = TESForm::LookupByID(ProjectileIDForm);
		auto player = PlayerCharacter::GetSingleton();

		bool wasPowerAttack = a_event->flags.all(TESHitEvent::Flag::kPowerAttack);
		bool wasSneakAttack = a_event->flags.all(TESHitEvent::Flag::kSneakAttack);
		bool wasBashAttack = a_event->flags.all(TESHitEvent::Flag::kBashAttack);
		bool wasHitBlocked = a_event->flags.all(TESHitEvent::Flag::kHitBlocked);
		static Timer timer = Timer(0.25);

		float attackerscale = get_visual_scale(attacker);
		float receiverscale = get_visual_scale(receiver) * GetSizeFromBoundingBox(receiver);

		float size_difference = attackerscale/receiverscale;

		// Apply it
		logger::info("Hit Name: {}", hitName);
		logger::info("Hit FormType: {}", RE::FormTypeToString(HitId->GetFormType()));
		float pushpower = GetPushPower(size_difference);
		if (attacker->IsPlayerRef()) {
			FormType formType = HitId->GetFormType();
			if (formType != FormType::Weapon) {
				return;
			}
			if (TinyCalamity_PushFromHit(attacker, receiver, wasPowerAttack, hitName)) {
				// Do stuff
			} else if (size_difference >= 4.0f) {
				if (wasPowerAttack || hitName.find("Bow") != std::string::npos) {
					size_difference *= 2.0f;
					pushpower *= 2.0f;
				}
				if (hitName.find("Bow") == std::string::npos) {
					shake_camera(attacker, size_difference * 0.20f, 0.35f);
				}
				PushForward(attacker, receiver, pushpower * 25.0f);
			}
		}
	}
}
