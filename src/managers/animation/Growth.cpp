#include "managers/animation/Utils/AnimationUtils.hpp"
#include "managers/animation/AnimationManager.hpp"
#include "managers/emotions/EmotionManager.hpp"
#include "managers/damage/CollisionDamage.hpp"
#include "managers/damage/LaunchActor.hpp"
#include "managers/animation/Growth.hpp"
#include "managers/GtsSizeManager.hpp"
#include "managers/InputManager.hpp"
#include "managers/CrushManager.hpp"
#include "magic/effects/common.hpp"
#include "managers/explosion.hpp"
#include "managers/audio/footstep.hpp"
#include "utils/actorUtils.hpp"
#include "managers/Rumble.hpp"
#include "data/persistent.hpp"
#include "managers/tremor.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"
#include "data/time.hpp"
#include "timer.hpp"
#include "node.hpp"

using namespace std;
using namespace SKSE;
using namespace RE;
using namespace Gts;

namespace {

	void SetHalfLife(Actor* actor, float value) {
		auto& Persist = Persistent::GetSingleton();
		auto actor_data = Persist.GetData(actor);
		if (actor_data) {
			actor_data->half_life = value;
		}
	}

	void CancelGrowth(Actor* actor) {
		std::string name = std::format("ManualGrowth_{}", actor->formID);
		TaskManager::Cancel(name);
		//SetHalfLife(actor, 1.0f);
	}

	void GrowthTask(Actor* actor) {
		if (!actor) {
			return;
		}
		double Start = Time::WorldTimeElapsed();
		ActorHandle gianthandle = actor->CreateRefHandle();
		std::string name = std::format("ManualGrowth_{}", actor->formID);

		float Volume = std::clamp(get_visual_scale(actor)/8.0f, 0.20f, 1.0f);
		Runtime::PlaySoundAtNode("growthSound", actor, Volume, 1.0f, "NPC Pelvis [Pelv]");

		//SetHalfLife(actor, 0.0f);
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
	void GtsGrowth_Moan(AnimationEventData& data) {
		PlayMoanSound(&data.giant, 1.0f);
	}
	void GtsGrowth_Mouth_Open(AnimationEventData& data) {
		Task_FacialEmotionTask_Moan(&data.giant, 1.25f, "GrowthMoan", 0.15f);
	}
	void GtsGrowth_Mouth_Close(AnimationEventData& data) {
	}
	void GTSGrowth_Enter(AnimationEventData& data) {
	}
	void GTSGrowth_SpurtStart(AnimationEventData& data) {
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

namespace Gts
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