#include "Managers/Animation/Shrink.hpp"

#include "Managers/Animation/AnimationManager.hpp"
#include "Magic/Effects/Common.hpp"
#include "Managers/Rumble.hpp"

using namespace GTS;

namespace {

	void SetHalfLife(Actor* actor, float value) {
		auto& Persist = Persistent::GetSingleton();
		auto actor_data = Persist.GetData(actor);
		if (actor_data) {
			actor_data->half_life = value;
		}
	}

	void ShrinkTask(Actor* actor) {
		if (!actor) {
			return;
		}
		double Start = Time::WorldTimeElapsed();
		ActorHandle gianthandle = actor->CreateRefHandle();
		std::string name = std::format("ManualShrink_{}", actor->formID);

		float scale = get_visual_scale(actor);
		float Volume = std::clamp(scale * 0.10f, 0.10f, 1.0f);
			
		Runtime::PlaySoundAtNode("GTSSoundShrink", actor, Volume, "NPC Pelvis [Pelv]");

		//SetHalfLife(actor, 0.0f);
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto caster = gianthandle.get().get();
			double timepassed = Time::WorldTimeElapsed() - Start;
			float elapsed = static_cast<float>(std::clamp(timepassed * AnimationManager::GetAnimSpeed(caster), 0.01, 1.2));
			float multiply = bezier_curve(elapsed, 0, 1.9f, 0.6f, 0, 2.0f, 1.0f);
			//                            ^value   x1  x2  x3  x4  i     k

			float caster_scale = get_visual_scale(caster);
			float stamina = std::clamp(GetStaminaPercentage(caster), 0.05f, 1.0f);

			float perk = Perk_GetCostReduction(caster);

			DamageAV(caster, ActorValue::kStamina, 0.15f * perk * caster_scale * stamina * TimeScale() * multiply);

			float shrink = CalcPower(caster, 0.0080f * stamina * multiply, 0.0f, false); // Is automatically *'d by scale through CalcPower()

			if (caster_scale > Minimum_Actor_Scale) {
				override_actor_scale(caster, -shrink, SizeEffectType::kNeutral);
			} else {
				override_actor_scale(caster, Minimum_Actor_Scale, SizeEffectType::kNeutral);
				set_target_scale(caster, Minimum_Actor_Scale);
				return false;
			}

			Rumbling::Once("ShrinkButton", caster, 2.0f * stamina, 0.05f, "NPC Pelvis [Pelv]",  0.0f);
			if (elapsed >= 0.99f) {
				return false;
			}
			return true;
		});
	}
	void GTSShrink_Enter(AnimationEventData& data) {
	}
	void GTSShrink_StartShrink(AnimationEventData& data) {
		ShrinkTask(&data.giant);
	}
	void GTSShrink_SlowShrink(AnimationEventData& data) {
	}
	void GTSShrink_StopShrink(AnimationEventData& data) {
	}
	void GTSShrink_Exit(AnimationEventData& data) {
	}
}

namespace GTS
{
	void AnimationShrink::RegisterEvents() {
		AnimationManager::RegisterEvent("GTSShrink_Enter", "Shrink", GTSShrink_Enter);
		AnimationManager::RegisterEvent("GTSShrink_StartShrink", "Shrink", GTSShrink_StartShrink);
		AnimationManager::RegisterEvent("GTSShrink_SlowShrink", "Shrink", GTSShrink_SlowShrink);
		AnimationManager::RegisterEvent("GTSShrink_StopShrink", "Shrink", GTSShrink_StopShrink);
		AnimationManager::RegisterEvent("GTSShrink_Exit", "Shrink", GTSShrink_Exit);
	}

	void AnimationShrink::RegisterTriggers() {
		AnimationManager::RegisterTrigger("TriggerShrink", "Shrink", "GTSBeh_Shrink_Trigger");
	}
}