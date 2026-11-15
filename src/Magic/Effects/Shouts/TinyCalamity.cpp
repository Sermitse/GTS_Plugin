#include "Magic/Effects/Shouts/TinyCalamity.hpp"
#include "Managers/Rumble.hpp"
#include "Managers/Perks/ShrinkingGaze.hpp"
#include "Managers/HighHeel.hpp"
#include "Config/Config.hpp"

using namespace GTS;

namespace {
	void RecordTinyCalamity(Actor* actor, float seconds_passed, float starting_duration, bool enable) {
		if (auto TranData = Transient::GetActorData(actor)) {
			TranData->TinyCalamityActive = enable;
			TranData->TinyCalamity_SecondsPassed = seconds_passed;
			TranData->TinyCalamity_StartingDuration = starting_duration;
		}
	}
	float GetSMTBonus(Actor* actor) {
		auto transient = Transient::GetActorData(actor);
		if (transient) {
			return transient->SMTBonusDuration;
		}
		return 0.0f;
	}

	float GetSMTPenalty(Actor* actor) {
		auto transient = Transient::GetActorData(actor);
		if (transient) {
			return transient->SMTPenaltyDuration;
		}
		return 0.0f;
	}

	void ResetDurationBonuses(Actor* actor) {
		auto transient = Transient::GetActorData(actor);
		if (transient) {
			transient->SMTBonusDuration = 0.0f;
			transient->SMTPenaltyDuration = 0.0f;
		}
	}

	void AdjustCalamityDuration(Actor* caster, ActiveEffect* Effect) {
		if (Runtime::HasPerkTeam(caster, Runtime::PERK.GTSPerkTinyCalamitySizeSteal)) {
			if (Effect) {
				float level = GetGtsSkillLevel(caster) - 85.0f;
				float extra = std::clamp(level, 0.0f, 15.0f) * 2.0f;
				Effect->duration += extra;
				// up to +30 seconds
			}
		}
	}

	void CapCasterSize(Actor* caster, float CasterScale, bool ShouldRun) {
		if (CasterScale < 1.0f) {
			set_target_scale(caster, 1.0f);
		} else if (CasterScale > 1.5f) {
			update_target_scale(caster, -0.0300f * GetCalamityPowerLevelScaled(caster, 2.0f), SizeEffectType::kNeutral);
			if (ShouldRun && caster->IsPlayerRef()) {
				Notify("Im getting too big, it becomes hard to handle such power.");
			}
		} // <- Disallow having it when scale is > natural scale * 1.50
	}
}

namespace GTS {

	std::string TinyCalamity::GetName() {
		return "TinyCalamity";
	}

	void TinyCalamity::OnStart() {
		auto caster = GetCaster();
		if (caster) {
			SetCalamityPowerLevel(caster, GetActiveEffect()->magnitude);

			float CalamityLevel = 1.0f;

			if (auto TranData = Transient::GetActorData(caster)) { 
				//Must use TranData directly because Calamity Check inside original function returns false
				//Because we check it too early for it to be True (magicEffect didn't start yet)
				CalamityLevel = TranData->TinyCalamity_PowerLevel;
			}
			float dur_Penalty = std::lerp(1.0f, 0.33f, (CalamityLevel - 1.0f) / 2.0f);

			if (CalamityLevel > 1.01f) {
				GetActiveEffect()->duration *= (dur_Penalty) + (Config::Advanced.bEndlessCalamity ? 999999.0f : 0.0f);
			}
			Cprint("Calamity Level: {}", CalamityLevel);
			RecordTinyCalamity(caster, GetActiveEffect()->elapsedSeconds, GetActiveEffect()->duration, true);
			if (caster->IsPlayerRef() && !Persistent::MSGSeenTinyCamity.value) {
				PrintMessageBox(TinyCalamityMessage);
				Persistent::MSGSeenTinyCamity.value = true;
			}
			
			Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundTinyCalamity, caster, 1.0f, "NPC COM [COM ]");
			AdjustCalamityDuration(caster, GetActiveEffect());
			auto node = find_node(caster, "NPC Root [Root]");
			StartShrinkingGaze(caster);

			if (node) {
				NiPoint3 position = node->world.translate;
				float scale = get_visual_scale(caster) * GetCalamityPowerLevelScaled(caster, 0.75f);
				TinyCalamityExplosion(caster, 84);

				SpawnParticle(caster, 6.00f, "GTS/Effects/TinyCalamity.nif", NiMatrix3(), position, scale * 3.0f, 7, nullptr); // Spawn
				Rumbling::For("TinyCalamityRumble", caster, 4.0f, 0.14f, "NPC COM [COM ]", 0.10f, 0.0f);
			}

			if (CalamityLevel > 1.1f) {
				Notify(CalamityLevel < 2.1f ? TinyCalamity_Word2 : TinyCalamity_Word3);
			}
		}
		
	}

	void TinyCalamity::OnUpdate() {

		auto caster = GetCaster();
		if (!caster) {
			return;
		}
		static Timer warningtimer = Timer(3.0);
		float CasterScale = get_target_scale(caster);
		auto effect = GetActiveEffect();
		RecordTinyCalamity(caster, effect->elapsedSeconds, effect->duration, true);
		if (float bonus = GetSMTBonus(caster); bonus > 0.5f) {
			effect->duration += bonus;
			ResetDurationBonuses(caster);
		}
		if (float penalty = GetSMTPenalty(caster); penalty > 0.5f) {
			effect->duration -= penalty;
			ResetDurationBonuses(caster);
		}
		CapCasterSize(caster, CasterScale, warningtimer.ShouldRun()); // Cap size between 1.0x and 1.5, not bigger than that.
	}

	void TinyCalamity::OnFinish() {
		auto caster = GetCaster();
		if (caster) {
			RecordTinyCalamity(caster, 0.0f, 0.0f, false);
			SetCalamityPowerLevel(caster, 1.0f);
			float CasterScale = get_target_scale(caster);
			float naturalscale = get_natural_scale(caster, true);
			if (CasterScale < naturalscale) {
				set_target_scale(caster, naturalscale);
			}
		}
	}
}
