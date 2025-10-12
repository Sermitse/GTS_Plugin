#include "Magic/Effects/Enchantments/Sword_Of_Size.hpp"
#include "Magic/Effects/Common.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"

namespace {

	float GetShrinkModifier(float value) {
		// https://www.desmos.com/calculator/ygoxbe7hjg
		float k = 0.9f;
		float a = 0.0f;
		float n = 0.7f;
		float s = 1.0f;
		float result = k*pow(s*(value-a), n);
		if (result < 1.0f) {
			return 1.0f;
		}
		return result;
	}

	float Ench_AbsorbSize_GetPower(float power) {
		float power_result = 1.0f;
		float reduction = GetShrinkModifier(power);
		power_result /= reduction;
		return power_result;
	}
}

namespace GTS {
	std::string SwordOfSize::GetName() {
		return "SwordOfSize";
	}

	SwordOfSize::SwordOfSize(ActiveEffect* effect) : Magic(effect) {
	}

	void SwordOfSize::OnStart() {
		auto caster = GetCaster();
		if (caster) {
			this->power = GetActiveEffect()->magnitude * 0.20f;
		}
		auto target = GetTarget();
		if (caster && target) {
			if (!IsEssential_WithIcons(caster, target)) {
				// Just spawn icon
			}
		}
	}

	void SwordOfSize::OnUpdate() {
		auto caster = GetCaster();
		if (!caster) {
			return;
		}
		auto target = GetTarget();
		if (!target) {
			return;
		}
		if (!IsEssential(caster, target)) {

			float gain_value = 0.02f;
			float ench_power = this->power;

			float calc_power = ench_power * Ench_AbsorbSize_GetPower(ench_power);
			float base_shrink = std::clamp(calc_power, 1.0f, 10.0f);
			float shrink_value = base_shrink * 3;

			// balanced around default value of 3.0f 

			if (target->IsDead()) {
				shrink_value *= 3.0f;
				gain_value *= 0.20f;
			}

			TransferSize(caster, target, false, shrink_value, gain_value, false, ShrinkSource::Enchantment);\
		}
	}

	void SwordOfSize::OnFinish() {
		auto caster = GetCaster();
		auto target = GetTarget();
		if (!caster) {
			return;
		}
		if (!target) {
			return;
		}
		if (!IsEssential(caster, target)) {
			Task_TrackSizeTask(caster, target, "Sword", false, 1.0f);
		}
	}
}

