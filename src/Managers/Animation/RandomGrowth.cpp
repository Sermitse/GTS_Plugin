#include "Managers/Animation/RandomGrowth.hpp"
#include "Managers/Animation/AnimationManager.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/RandomGrowth.hpp"
#include "Managers/Rumble.hpp"

#include "Magic/Effects/Common.hpp"

#include "Managers/Audio/MoansLaughs.hpp"

using namespace GTS;

namespace {

	constexpr float anim_fps_speed = 30.0f;

	GrowthAnimation GetGrowthType(Actor* giant) { // Used as a way to read which exact Growth was triggered (it is full RNG on Behavior side)
		int growthtype = 0;
		giant->GetGraphVariableInt("GTS_Growth_Roll", growthtype);

		GrowthAnimation Anim = static_cast<GrowthAnimation>(growthtype);

		return Anim;
	}

	float get_growth_multiplier(Actor* giant) {
		int growth_roll = static_cast<int>(GetGrowthType(giant));
		float multiplier = 1.0f;

		if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkRandomGrowthTerror)) {
			multiplier = 1.3f;
		}

		switch (growth_roll) {
			case 1:
				return 0.38f * multiplier * 1.45f; // ~62% without * 1.25
			case 5:
			case 2:
			 	return 0.26f * multiplier * 1.40f; // ~40% without * 1.25
			case 6:
			case 3:
				return 0.28f * multiplier * 1.35f; // ~42% without * 1.25
			case 4:
				return 0.34f * multiplier * 1.25f; // ~62% without * 1.25
			default: {} ;
		}

		return 0.9f * multiplier;
	}

	float get_growth_formula(Actor* giant, float elapsed, GrowthAnimation anim) {
		float formula = 0.0f;
		switch (anim) {
			case GrowthAnimation::None:
				log::info("Formula = 0");
				return 0.0f;
			break;
			case GrowthAnimation::Growth_1:
				formula = bezier_curve(elapsed * 0.32f, 0.2f, 2.1f, -0.2f, 0, 3.0f, 7.0f); // https://www.desmos.com/calculator/aqbzm5e97p
			break;
			case GrowthAnimation::Growth_5: // 5 is copy-pasted 2 for now
			case GrowthAnimation::Growth_2:
				formula = bezier_curve(elapsed * 0.33f, 0.2f, 1.9f, 0, 0, 3.0f, 4.0f);  // https://www.desmos.com/calculator/reqejljy19
				//log::info("Formula = 2");
			break;
			case GrowthAnimation::Growth_6: // 6 is copy-pasted 3 for now
			case GrowthAnimation::Growth_3:
				elapsed = std::clamp(elapsed, 0.0f, 2.0f); // Fix formula going into positives, causing growth once everything is done
				formula = bezier_curve(elapsed * 0.48f, 0.4f, 3, 0, 0, 3.0f, 1.9f); // https://www.desmos.com/calculator/liko5e9kca
				//log::info("Formula = 3");
			break;
			case GrowthAnimation::Growth_4:
				formula = bezier_curve(elapsed * 0.275f, 0, 3.5f, 0.2f, 0, 1.0f, 0.68f); // https://www.desmos.com/calculator/7ynul48a93
				//log::info("Formula = 4");
			break;
		}
		return formula;
	}

	void GrowthTask(Actor* actor, float growth_mult) {
		if (!actor) {
			return;
		}
		
		ActorHandle gianthandle = actor->CreateRefHandle();
		std::string name = std::format("RandomGrowth_{}", actor->formID);
		
		GrowthAnimation GrowthType = GetGrowthType(actor);

		double Start = Time::WorldTimeElapsed();

		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giant = gianthandle.get().get();
			double timepassed = Time::WorldTimeElapsed() - Start;
			float animspeed = AnimationManager::GetAnimSpeed(giant);

			float elapsed = static_cast<float>(std::clamp(timepassed * animspeed, 0.0, 4.4));
			float gain = std::clamp(get_growth_formula(giant, elapsed, GrowthType), -0.01f, 1.0f);

			float growth = CalcPower(actor, 0.0080f * growth_mult * gain * animspeed, 0.0f, false);

			if (gain > 0) {
				override_actor_scale(giant, growth, SizeEffectType::kGrow);
				RandomGrowth::RestoreStats(giant, gain);
			}
			
			Rumbling::Once("RandomGrowth", giant, 2.0f * gain, 0.0f, "NPC Pelvis [Pelv]", 0.0f);

			//log::info("elapsed: {}, mult: {}, IsGrowing: {}", elapsed, gain, IsGrowing(giant));
			if (!IsActionOnCooldown(giant, CooldownSource::Misc_GrowthSound)) {
				ApplyActionCooldown(giant, CooldownSource::Misc_GrowthSound);

				float Volume = std::clamp(get_visual_scale(actor)/8.0f, 0.20f, 1.0f);
				Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundGrowth, actor, Volume * gain, "NPC Pelvis [Pelv]");
			}
			
			if (!IsGrowing(giant) || elapsed > 1.8f && gain < 0.0f) {
				return false;
			}
			return true;
		});
	}
	void GTS_RandomGrowth_Start(AnimationEventData& data) {
		GrowthTask(&data.giant, get_growth_multiplier(&data.giant));
	}

	void GTS_RandomGrowth_Peak(AnimationEventData& data) {
		Actor* giant = &data.giant;

		Sound_PlayMoans(giant, 1.0f, 0.14f, EmotionTriggerSource::Growth);
		Task_FacialEmotionTask_Moan(giant, 1.75f, "RandomGrow");

		if (Runtime::HasPerkTeam(giant, Runtime::PERK.GTSPerkRandomGrowthTerror)) {
			for (auto tiny: find_actors()) {
				if (tiny && tiny != giant) {
					if (IsHostile(giant, tiny) || IsHostile(tiny, giant)) {
						NiPoint3 distance_a = giant->GetPosition();
						NiPoint3 distance_b = tiny->GetPosition();
						float distance = (distance_a - distance_b).Length();
						if (distance <= 212 * get_visual_scale(giant)) {
							ChanceToScare(giant, tiny, 4, 3, false);
						}
					}
				}
			}
		}
	}
	void GTS_RandomGrowth_Taper(AnimationEventData& data) {}
	void GTS_RandomGrowth_End(AnimationEventData& data) {}

	void GTS_ResetVars(const AnimationEventData& data) {
        Actor* giant = &data.giant;
        std::string name = std::format("ResetIGrowth_{}", giant->formID);
		ActorHandle gianthandle = giant->CreateRefHandle();

		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giantref = gianthandle.get().get();

			if (!giantref) {
				return false; // end task in that case
			}

            if (!IsGrowing(giantref)) {
                giantref->SetGraphVariableInt("GTS_Growth_Roll", 0);
                return false;
            }
			// All good try another frame
			return true;
		});
    }
}

namespace GTS
{
	void Animation_RandomGrowth::RegisterEvents() {
		AnimationManager::RegisterEvent("GTS_RandomGrowth_Start", "RandomGrowth", GTS_RandomGrowth_Start);
        AnimationManager::RegisterEvent("GTS_RandomGrowth_Peak", "RandomGrowth", GTS_RandomGrowth_Peak);
        AnimationManager::RegisterEvent("GTS_RandomGrowth_Taper", "RandomGrowth", GTS_RandomGrowth_Taper);
        AnimationManager::RegisterEvent("GTS_RandomGrowth_End", "RandomGrowth", GTS_RandomGrowth_End);
		AnimationManager::RegisterEvent("GTS_ResetVars", "RandomGrowth", GTS_ResetVars);
	}

	void Animation_RandomGrowth::RegisterTriggers() {
		AnimationManager::RegisterTrigger("StartRandomGrowth", "RandomGrowth", "GTSBEH_Grow_Random");
	}
}