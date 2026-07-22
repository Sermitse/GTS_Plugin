#include "Magic/Effects/Shouts/TinyCalamity.hpp"
#include "Managers/Rumble.hpp"
#include "Managers/Perks/ShrinkingGaze.hpp"
#include "Managers/HighHeel.hpp"

using namespace GTS;

namespace {
	void RecordTinyCalamity(Actor* actor, bool enable) {
		if (auto TranData = Transient::GetActorData(actor)) {
			TranData->TinyCalamityActive = enable;
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

	void SpawnFootParticles(Actor* caster) {
		const float scale = get_visual_scale(caster);
		for (auto foot: {"NPC R Foot [Rft ]","NPC L Foot [Lft ]"}) {
			if (auto node = find_node(caster, foot)) {
				auto pos = node->world.translate;
				pos.z -= HighHeelManager::GetInitialHeelHeight(caster) * 100.0f * scale;
				SpawnCustomParticle(caster, ParticleType::Red, pos, foot, scale * 0.5f);
			}
		}
	}

	void CapCasterSize(Actor* caster, float CasterScale, bool ShouldRun) {
		if (CasterScale < 1.0f) {
			set_target_scale(caster, 1.0f);
		} else if (CasterScale > 1.5f) {
			update_target_scale(caster, -0.0300f, SizeEffectType::kNeutral);
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
			RecordTinyCalamity(caster, true);
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
				float scale = get_visual_scale(caster);
				TinyCalamityExplosion(caster, 84);

				SpawnParticle(caster, 6.00f, "GTS/Effects/TinyCalamity.nif", NiMatrix3(), position, scale * 3.0f, 7, nullptr); // Spawn
				Rumbling::For("TinyCalamityRumble", caster, 4.0f, 0.14f, "NPC COM [COM ]", 0.10f, 0.0f);
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

		if (float bonus = GetSMTBonus(caster); bonus > 0.5f) {
			GetActiveEffect()->duration += bonus;
			ResetDurationBonuses(caster);
		}
		if (float penalty = GetSMTPenalty(caster); penalty > 0.5f) {
			GetActiveEffect()->duration -= penalty;
			ResetDurationBonuses(caster);
		}
		CapCasterSize(caster, CasterScale, warningtimer.ShouldRun()); // Cap size between 1.0x and 1.5, not bigger than that.
	}

	void TinyCalamity::OnFinish() {
		auto caster = GetCaster();
		if (caster) {
			RecordTinyCalamity(caster, false);
			float CasterScale = get_target_scale(caster);
			float naturalscale = get_natural_scale(caster, true);
			if (CasterScale < naturalscale) {
				set_target_scale(caster, naturalscale);
			}
		}
	}
}
