
#include "Magic/Effects/Common.hpp"
#include "Magic/Effects/Spells/Shrink_Rune.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"

using namespace GTS;

namespace {

	// Currently this magic is unused

	void ShrinkByOverTime(Actor* tiny, float over_time, float by) {
		ActorHandle tinyHandle = tiny->CreateRefHandle();
		std::string name = std::format("ShrinkToSize_{}", tiny->formID);

		if (tiny->IsDead()) {
			by *= 1.75f;
		}
		float bb = std::clamp(GetSizeFromBoundingBox(tiny), 0.6f, 100.f);
		double Start = Time::WorldTimeElapsed();

		TaskManager::Run(name, [=](auto& progressData) {
			Actor* actor = tinyHandle.get().get();
			if (!actor) {
				return false;
			}

			float Scale = get_visual_scale(actor);
			double End = Time::WorldTimeElapsed();

			if (End - Start < over_time) {
				override_actor_scale(actor, -by * (0.0166f/bb) * TimeScale(), SizeEffectType::kNeutral);
				if (get_target_scale(actor) < SHRINK_TO_NOTHING_SCALE / bb) {
					set_target_scale(actor, SHRINK_TO_NOTHING_SCALE / bb);
					return false;
				}
				return true;
			} else {
				return false;
			}
		});
	}

	void ResetMovementSlowdown(Actor* tiny) {
		auto transient = Transient::GetActorData(tiny);
		if (transient) {
			transient->MovementSlowdown = 1.0f;
		}
	}
	void SetMovementSlowdown(Actor* giant, Actor* tiny) { 
		auto transient = Transient::GetActorData(tiny);
		if (transient) {
			float slow = 0.50f;
			if (Runtime::HasPerkTeam(giant, "GTSPerkShrinkAdept")) {
				slow += 0.05f;
			}
			if (Runtime::HasPerkTeam(giant, "GTSPerkShrinkExpert")) {
				slow += 0.05f;
			}
			transient->MovementSlowdown = slow;
		}
	}
}


namespace GTS {
	std::string ShrinkRune::GetName() {
		return "ShrinkRune";
	}

	ShrinkRune::ShrinkRune(ActiveEffect* effect) : Magic(effect) {
		this->power = GetActiveEffect()->magnitude * 0.015;
	}

	void ShrinkRune::OnStart() {
		auto caster = GetCaster();
		if (!caster) {
			return;
		}
		auto target = GetTarget();
		if (!target) {
			return;
		}
		if (target != caster) {
			if (!IsEssential_WithIcons(caster, target)) {
				float sizediff = GetSizeDifference(caster, target, SizeType::VisualScale, true, false);
				if (this->power >= 18.00f && sizediff > 4.0f) {
					StaggerActor(caster, target, 100.0f);
				}
				SetMovementSlowdown(caster, target);
			}
			log::info("Apply Shrink Rune To {}", target->GetDisplayFullName());
			Task_TrackSizeTask(caster, target, "Spell", true, this->time_mult);
			ShrinkByOverTime(target, 0.25, this->power);
			ModSizeExperience(caster, 0.02);
		}
	}

	void ShrinkRune::OnUpdate() {}

	void ShrinkRune::OnFinish() {
		auto caster = GetCaster();
		auto target = GetTarget();

		if (!caster) {
			return;
		}
		if (!target) {
			return;
		}
		if (!IsEssential(caster, target)) {
			
			ResetMovementSlowdown(target);
		}
	}
}
