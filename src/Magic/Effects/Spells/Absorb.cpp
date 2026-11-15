#include "Magic/Effects/Spells/Absorb.hpp"
#include "Magic/Effects/Common.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"

namespace GTS {

	Absorb::Absorb(ActiveEffect* effect) : Magic(effect) {}

	std::string Absorb::GetName() {
		return "Absorb";
	}

	void Absorb::OnStart() {
		auto target = GetTarget();
		if (!target) {
			return;
		} 
		auto caster = GetCaster();
		if (!caster) {
			return;
		}
		auto effect = GetActiveEffect();
		if (effect) {
			this->power = std::min(effect->magnitude, 3.0f);
			this->time_mult = 1.25f / this->power;
		}
		Cprint("Absorb Power:{}", this->power);
		if (!IsEssential_WithIcons(caster, target)) {
			StaggerActor(caster, target, 0.25f * get_scale_difference(caster, target, SizeType::VisualScale, true, false));
			target->Attacked(caster);
		}
		if (auto transient = Transient::GetActorData(target)) {
			transient->ShrinkWeakness = std::min(transient->ShrinkWeakness + 0.15f * this->power, 1.0f);
		}
	}

	void Absorb::OnUpdate() {
		constexpr float SMT_BONUS = 1.0f;
		auto caster = GetCaster();
		if (!caster) {
			return;
		}
		auto target = GetTarget();
		if (!target) {
			return;
		}

		if (target == caster) {
			return;
		}
		if (IsEssential(caster, target)) {
			return; // Disallow shrinking Essentials
		}
		float size_difference = std::clamp(get_scale_difference(caster, target, SizeType::VisualScale, true, false), 1.0f, 3.0f);

		if (TinyCalamityActive(caster)) {
			size_difference += SMT_BONUS;
		} // More shrink with SMT

		const bool isTooWeak = GetHealthPercentage(target) < 0.15f || target->IsDead();
		const auto effects = GetActiveEffect();
		bool firstTicks = false;
		if (effects) {
			firstTicks = GetActiveEffect()->elapsedSeconds <= 1.0f;
		}
		target->Attacked(caster); // make it seen as hostile action

		if (isTooWeak || firstTicks) {
			float shrink_power = firstTicks ? 13.5f : 0.125f;
			shrink_power *= size_difference * this->power;
			float gain_size = 0.0025f * this->power;

			if (ShrinkToNothing(caster, target, true, this->time_mult, 1.0f, false, false, ShrinkToNothingSource::ShoutAbsorb) >= ShrinkToNothingState::TickProtection) { 
				// Sending ShoutAbsorb to function = chance to receive more size xp and grow even bigger
			} else {
				TransferSize(caster, target, true, shrink_power, gain_size, false, ShrinkSource::Magic);
			}
		}
	}
	

	void Absorb::OnFinish() {
		auto caster = GetCaster();
		auto target = GetTarget();
		if (!caster) {
			return;
		} 
		if (!target) {
			return;
		}
		Task_TrackSizeTask(caster, target, ShrinkToNothingSource::ShoutAbsorb, false, this->time_mult);
	}
}
