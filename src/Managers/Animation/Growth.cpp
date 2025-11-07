#include "Managers/Animation/Growth.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Rumble.hpp"

#include "Magic/Effects/Common.hpp"

#include "Managers/Audio/MoansLaughs.hpp"

using namespace GTS;

namespace {

	void DelayedMoan(Actor* actor) { // Have to do it this way since Animator forgot annotations
		if (actor) {
			double Start = Time::WorldTimeElapsed();
			ActorHandle gianthandle = actor->CreateRefHandle();
			std::string name = std::format("DelayMoan_{}", actor->formID);

			TaskManager::Run(name, [=](auto& progressData) {
				if (!gianthandle) {
					return false;
				}
				auto giant = gianthandle.get().get();
				double timepassed = Time::WorldTimeElapsed() - Start;
				if (timepassed >= 0.15f / AnimationManager::GetAnimSpeed(giant)) {
					Task_FacialEmotionTask_Moan(giant, 1.25f, "GrowthMoan", 0.15f);
					Sound_PlayMoans(giant, 1.0f, 0.14f, EmotionTriggerSource::Growth, CooldownSource::Emotion_Voice_Long);
					return false;
				}
				return true;
			});
		}
	}

	void GrowthTask(Actor* actor) {
		if (actor) {
			double Start = Time::WorldTimeElapsed();
			ActorHandle gianthandle = actor->CreateRefHandle();
			std::string name = std::format("ManualGrowth_{}", actor->formID);

			float Volume = std::clamp(get_visual_scale(actor)/8.0f, 0.20f, 1.0f);
			Runtime::PlaySoundAtNode("GTSSoundGrowth", actor, Volume, "NPC Pelvis [Pelv]");

			TaskManager::Run(name, [=](auto& progressData) {
				if (!gianthandle) {
					return false;
				}
				auto caster = gianthandle.get().get();
				double timepassed = Time::WorldTimeElapsed() - Start;
				float animspeed = AnimationManager::GetAnimSpeed(caster);
				float elapsed = static_cast<float>(std::clamp(timepassed * animspeed, 0.01, 1.2));
				float multiply = bezier_curve(elapsed, 0, 1.9f, 0.6f, 0, 2.0f, 1.0f);
				//                            ^value   x1  x2  x3  x4  i     k

				float caster_scale = get_visual_scale(caster);
				float stamina = std::clamp(GetStaminaPercentage(caster), 0.05f, 1.0f);

				float perk = Perk_GetCostReduction(caster);  
				
				DamageAV(caster, ActorValue::kStamina, 0.60f * perk * caster_scale * stamina * TimeScale() * multiply);

				float modify = CalcPower(actor, 0.0080f * stamina * multiply * animspeed, 0.0f, false);

				override_actor_scale(caster, modify, SizeEffectType::kGrow);
				// value*scale ^  ; ^ static value, not affected by scale
				
				Rumbling::Once("GrowButton", caster, 2.0f * stamina, 0.05f, "NPC Pelvis [Pelv]", 0.0f);
				if (elapsed >= 0.99f) {
					//SetHalfLife(caster, 1.0f);
					return false;
				}
				return true;
			});
		}
	}
	void GtsGrowth_Moan(AnimationEventData& data) {
	}
	void GtsGrowth_Mouth_Open(AnimationEventData& data) {
	}
	void GtsGrowth_Mouth_Close(AnimationEventData& data) {
	}
	void GTSGrowth_Enter(AnimationEventData& data) {
	}
	void GTSGrowth_SpurtStart(AnimationEventData& data) {
		DelayedMoan(&data.giant);
		GrowthTask(&data.giant);
	}
	void GTSGrowth_SpurtSlowdownPoint(AnimationEventData& data) {
	}
	void GTSGrowth_SpurtStop(AnimationEventData& data) {
		//CancelGrowth(&data.giant);
	}
	void GTSGrowth_Exit(AnimationEventData& data) {
	}
}

namespace GTS
{
	void AnimationGrowth::RegisterEvents() {
		AnimationManager::RegisterEvent("GtsGrowth_Moan", "Growth", GtsGrowth_Moan);
		AnimationManager::RegisterEvent("GtsGrowth_Mouth_Open", "Growth", GtsGrowth_Mouth_Open);
		AnimationManager::RegisterEvent("GtsGrowth_Mouth_Close", "Growth", GtsGrowth_Mouth_Close);

		AnimationManager::RegisterEvent("GTSGrowth_Enter", "Growth", GTSGrowth_Enter);
		AnimationManager::RegisterEvent("GTSGrowth_SpurtStart", "Growth", GTSGrowth_SpurtStart);
		AnimationManager::RegisterEvent("GTSGrowth_SpurtSlowdownPoint", "Growth", GTSGrowth_SpurtSlowdownPoint);
		AnimationManager::RegisterEvent("GTSGrowth_SpurtStop", "Growth", GTSGrowth_SpurtStop);
		AnimationManager::RegisterEvent("GTSGrowth_Exit", "Growth", GTSGrowth_Exit);
	}

	void AnimationGrowth::RegisterTriggers() {
		AnimationManager::RegisterTrigger("TriggerGrowth", "Growth", "GTSBeh_Grow_Trigger");
	}
}