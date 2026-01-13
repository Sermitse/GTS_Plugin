#include "Managers/Animation/HugHeal.hpp"

#include "Config/Config.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/HugShrink.hpp"
#include "Managers/Rumble.hpp"

#include "Magic/Effects/Common.hpp"

#include "Managers/Audio/MoansLaughs.hpp"

using namespace GTS;

namespace {

	void ActivateEmotions(Actor* actor, bool toggle) {
		float p_1 = 1.0f;
		float p_2 = 0.75f;
		int rng = RandomInt(0, 8);
		if (!toggle) {
			p_1 = 0.0f;
			p_2 = 0.0f;
		} if (rng <= 1) {
			Sound_PlayMoans(actor, 1.0f, 0.14f, EmotionTriggerSource::Vore);
		}
		AdjustFacialExpression(actor, 0, p_1, CharEmotionType::Modifier); // blink L
		AdjustFacialExpression(actor, 1, p_1, CharEmotionType::Modifier); // blink R
		AdjustFacialExpression(actor, 0, p_2, CharEmotionType::Phenome);
	}

	void AbortHugAnimation_Friendly(Actor* giant) {
		auto tiny = HugShrink::GetHuggiesActor(giant);
		if (tiny) {
			EnableCollisions(tiny);
			SetBeingHeld(tiny, false);
			UpdateFriendlyHugs(giant, tiny, true); // set GTS_IsFollower (tiny) and GTS_HuggingTeammate (GTS) bools to false
			Anims_FixAnimationDesync(giant, tiny, true); // reset anim speed override so .dll won't use it
		}
		SetSneaking(giant, false, 0);
		
		std::string name_normal = std::format("Huggies_{}", giant->formID);
		std::string name_forced = std::format("Huggies_Forced_{}", giant->formID);

		TaskManager::Cancel(name_normal);
		TaskManager::Cancel(name_forced);
		HugShrink::Release(giant);
	}

    bool Hugs_RestoreHealth(Actor* giantref, Actor* tinyref) {
		static Timer HeartTimer = Timer(0.5);
		float hp = GetAV(tinyref, ActorValue::kHealth);
		float maxhp = GetMaxAV(tinyref, ActorValue::kHealth);
		
		bool Healing = AnimationVars::Hug::IsHugHealing(giantref);

		const bool Teammate = IsTeammate(tinyref);
		const bool Player = tinyref->formID == 0x14;
		const bool IsPlayerOrMate = (Player || Teammate);
		const bool BothTeammates = IsTeammate(giantref) && Teammate;
		
		const bool ShouldContinue = (BothTeammates || IsPlayerOrMate);
		
		if (Healing && HeartTimer.ShouldRunFrame()) {
			SpawnHearts(giantref, tinyref, 0.0f, 2.4f, true);
		}

		if (!Healing && hp >= maxhp) {

			if (ShouldContinue && !Config::Gameplay.ActionSettings.bHugsStopAtFullHP) {
				return true;
			}

			AbortHugAnimation(giantref, tinyref);

			if (giantref->formID == 0x14) {
				Notify("{} health is full", tinyref->GetDisplayFullName());
			}
			return false;

		} 

		if (giantref->formID == 0x14) {
			float sizedifference = get_visual_scale(giantref)/get_visual_scale(tinyref);
			shake_camera(giantref, 0.30f * sizedifference, 0.05f);
		} else {
			Rumbling::Once("HugSteal", giantref, Rumble_Hugs_Heal, 0.02f);
		}
		tinyref->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kHealth, maxhp * 0.004f * 0.15f * TimeScale());

		if (!Healing) {
			return false;
		}

		return true;
	}

    void HealOtherTask(Actor* giant, Actor* tiny) {
		if (!giant) {
			return;
		}
		if (!tiny) {
			return;
		}
		std::string name = std::format("Huggies_Heal_{}", giant->formID);
		ActorHandle gianthandle = giant->CreateRefHandle();
		ActorHandle tinyhandle = tiny->CreateRefHandle();
		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			if (!tinyhandle) {
				return false;
			}
			auto giantref = gianthandle.get().get();
			auto tinyref = tinyhandle.get().get();

			float sizedifference = get_scale_difference(giantref, tinyref, SizeType::VisualScale, false, true);
			float threshold = 3.0f;
			float stamina = 0.35f;

			if (Runtime::HasPerkTeam(giantref, Runtime::PERK.GTSPerkHugsGreed)) {
				stamina *= 0.75f;
			}
			stamina *= Perk_GetCostReduction(giantref);

			if (!AnimationVars::Hug::IsHugHealing(giantref) && (sizedifference >= threshold || sizedifference < Action_Hug)) {
				SetBeingHeld(tinyref, false);
				AbortHugAnimation(giantref, tinyref);
				if (giantref->formID == 0x14) {
					shake_camera(giantref, 0.50f, 0.15f);
					Notify("It's difficult to gently hug {}", tinyref->GetDisplayFullName());
				}
				return false;
			}
			DamageAV(tinyref, ActorValue::kStamina, -(0.45f * TimeScale())); // Restore Tiny stamina
			DamageAV(giantref, ActorValue::kStamina, 0.25f * stamina * TimeScale()); // Damage GTS Stamina

			return Hugs_RestoreHealth(giantref, tinyref);

		});
	}

    void GTS_Hug_Heal(AnimationEventData& data) {
        auto huggedActor = HugShrink::GetHuggiesActor(&data.giant);
		if (huggedActor) {
			HealOtherTask(&data.giant, huggedActor);
		}
    }

	void GTS_Hug_Release(AnimationEventData& data) {AbortHugAnimation_Friendly(&data.giant);}

	void GTS_Hug_Moan_Tiny(AnimationEventData& data) {ActivateEmotions(&data.giant, true);}
	void GTS_Hug_Moan_Tiny_End(AnimationEventData& data) {ActivateEmotions(&data.giant, false);}
}


namespace GTS {
    void HugHeal::RegisterEvents() {
        AnimationManager::RegisterEvent("GTS_Hug_Heal", "Hugs", GTS_Hug_Heal);
		AnimationManager::RegisterEvent("GTS_Hug_Release", "Hugs", GTS_Hug_Release);
		AnimationManager::RegisterEvent("GTS_Hug_Moan_Tiny", "Hugs", GTS_Hug_Moan_Tiny);
		AnimationManager::RegisterEvent("GTS_Hug_Moan_Tiny_End", "Hugs", GTS_Hug_Moan_Tiny_End);
    }

    void HugHeal::RegisterTriggers() {
		AnimationManager::RegisterTrigger("Huggies_Heal", "Hugs", "GTSBEH_HugHealStart_A");
		AnimationManager::RegisterTrigger("Huggies_Heal_Victim_F", "Hugs", "GTSBEH_HugHealStart_Fem_V");
		AnimationManager::RegisterTrigger("Huggies_Heal_Victim_M", "Hugs", "GTSBEH_HugHealStart_Mal_V");
	}
}