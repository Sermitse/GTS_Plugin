#include "Managers/GtsSizeManager.hpp"
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
}

namespace GTS {

	HitManager& HitManager::GetSingleton() noexcept {
		static HitManager instance;
		return instance;
	}

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

		if (HasSMT(player)) {
			size_difference += 3.0f;
		}

		// Apply it
		float pushpower = GetPushPower(size_difference);
		if (attacker->formID == 0x14 && size_difference >= 4.0f) {
			FormType formType = HitId->GetFormType();
			if (formType != FormType::Weapon) {
				return;
			}
			if (wasPowerAttack || hitName.find("Bow") != std::string::npos) {
				size_difference *= 2.0f;
				pushpower *= 2.0f;
			}
			if (hitName.find("Bow") == std::string::npos) {
				shake_camera(attacker, size_difference * 0.20f, 0.35f);
			}
			PushForward(attacker, receiver, pushpower * 25.0f);
			//log::info("Size difference is met, pushing actor away");
		}
	}
}
