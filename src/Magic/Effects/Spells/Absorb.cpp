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
		if (!IsEssential_WithIcons(caster, target)) {
			StaggerActor(caster, target, 0.25f * GetSizeDifference(caster, target, SizeType::VisualScale, true, false));
			Attacked(target, caster);
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

		float size_difference = std::clamp(GetSizeDifference(caster, target, SizeType::VisualScale, true, false), 1.0f, 3.0f);

		if (HasSMT(caster)) {
			size_difference += SMT_BONUS;
		} // More shrink with SMT

		float shrink_power = 4.5f * size_difference;
		float gain_size = 0.0025f;

		if (target->IsDead()) {
			shrink_power *= 3.0f;
			gain_size *= 0.20f;
		}

		
		Attacked(target, caster);// make it seen as hostile action

		if (ShrinkToNothing(caster, target, false, 1.0f)) { // chance to receive more size xp and grow even bigger
			AbsorbShout_BuffCaster(caster, target);
		} else {
			TransferSize(caster, target, true, shrink_power, gain_size, false, ShrinkSource::Magic);
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
		Task_TrackSizeTask(caster, target, "Absorb", false, 1.0f);
	}
}
