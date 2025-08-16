#include "Magic/Effects/Potions/GrowthPotion.hpp"
#include "Magic/Effects/Common.hpp"
#include "Managers/Rumble.hpp"
#include "Managers/Audio/MoansLaughs.hpp"

using namespace GTS;

namespace {

	void PlayGrowthAudio(Actor* giant, bool checkTimer) {
		if (checkTimer) {
			
			float scale = get_visual_scale(giant);

			float falloff = 0.16f * scale;
			float Volume = std::clamp(scale/10.0f, 0.20f, 2.0f);

			Rumbling::Once("GrowthPotion", giant, 2.0f, 0.05f);

			Runtime::PlaySoundAtNode_FallOff("GTSSoundGrowth", giant, Volume, "NPC Pelvis [Pelv]", falloff);
		}
	}

}

namespace GTS {
	std::string GrowthPotion::GetName() {
		return "GrowthPotion";
	}

	GrowthPotion::GrowthPotion(ActiveEffect* effect) : Magic(effect) {

		auto base_spell = GetBaseEffect();

		if (base_spell == Runtime::GetMagicEffect("GTSPotionEffectGrowthWeak")) {
			this->power = 1.0f;
		} else if (base_spell == Runtime::GetMagicEffect("GTSPotionEffectGrowthNormal")) {
			this->power = 1.5f;
		} else if (base_spell == Runtime::GetMagicEffect("GTSPotionEffectGrowthStrong")) {
			this->power = 2.0f;
		} else if (base_spell == Runtime::GetMagicEffect("GTSPotionEffectGrowthExtreme")) {
			this->power = 2.5f;
		} else if (base_spell == Runtime::GetMagicEffect("GTSAlchEffectGrowth")) {
			RecordPotionMagnitude(GetActiveEffect(), this->power, 1.0f);
		}
	}

	void GrowthPotion::OnStart() {
		auto caster = GetCaster();
		if (!caster) {
			return;
		}

		if (this->power >= 2.0f) {
			bool Blocked = IsActionOnCooldown(caster, CooldownSource::Emotion_Moan);
			if (!Blocked) {
				Sound_PlayMoans(caster, 1.0f, 0.14f, EmotionTriggerSource::Growth, CooldownSource::Emotion_Voice_Long);
				ApplyActionCooldown(caster, CooldownSource::Emotion_Moan);
			}
			if (caster->formID == 0x14) {
				shake_camera(caster, 0.75f, 0.55f);
			}
		}

		Potion_SetUnderGrowth(caster, true);
		PlayGrowthAudio(caster, true);
		Potion_Penalty(caster);
	}

	void GrowthPotion::OnUpdate() {
		float BASE_POWER = 0.000128f * this->power;

		auto caster = GetCaster();
		if (!caster) {
			return;
		}

		PlayGrowthAudio(caster, this->timer.ShouldRun());

		float HP = GetMaxAV(caster, ActorValue::kHealth) * 0.00020f * this->power;
		caster->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kHealth, HP * TimeScale());

		float Power = BASE_POWER * 1.5f;

		Grow(caster, Power, 0.0f);
		Rumbling::Once("GrowthPotion", caster, 1.0f, 0.10f, "NPC COM [COM ]", 0.0f);

		//log::info("This Power: {}", this->power);
		//log::info("Enlarging {} with the power of {}", caster->GetDisplayFullName(), Power);
	}

	void GrowthPotion::OnFinish() {
		auto caster = GetCaster();
		if (caster) {
			Potion_SetUnderGrowth(caster, false);
		}
	}
}
