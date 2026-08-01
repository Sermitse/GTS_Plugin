#include "Magic/Effects/Spells/SlowGrow.hpp"

#include "Config/Config.hpp"

#include "Magic/Effects/Common.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Managers/Rumble.hpp"

#include "Managers/Audio/MoansLaughs.hpp"

using namespace GTS;

namespace {

	constexpr float BASE_POWER = 0.000025f; 	// Default growth over time.
	constexpr float DUAL_CAST_BONUS = 2.25f;
	constexpr float MOAN_CHANCE = 25.0f; 		// In %
	constexpr float EMPOWER_CHANCE = 75.0f; 	// in%

	void PerformMoanAndParticle(Actor* caster, float& power, float& shakePower) {
		if (caster && IsFemale(caster) && !IsActionOnCooldown(caster, CooldownSource::Emotion_Moan) && RandomBool(EMPOWER_CHANCE)) {
			for (auto Foot: {"NPC L Foot [Lft ]", "NPC R Foot [Rft ]"}) {
				auto FootNode = find_node(caster, Foot);
				if (FootNode) {
					SpawnCustomParticle(caster, ParticleType::Green, FootNode->world.translate, Foot, 0.75f * get_visual_scale(caster));
				}
			}
			shakePower *= 2.5f; power *= 320.0f; // First empower
			if (Config::Audio.bSlowGrowMoans && RandomBool(MOAN_CHANCE)) {
				SpawnCustomParticle(caster, ParticleType::Green, NiPoint3(), "NPC COM [COM ]", get_visual_scale(caster) * 1.5f);
				Task_FacialEmotionTask_Moan(caster, 1.0f + RandomFloat(0.0f, 0.25f), "SlowGrow");
				float MoanVolume = std::clamp(get_visual_scale(caster)/4.0f, 0.5f, 1.0f);
				Sound_PlayMoans(caster, MoanVolume, 0.14f, EmotionTriggerSource::Growth, CooldownSource::Emotion_Voice_Long);
				shakePower *= 3.0f; power *= 2.5f;  // Second empower if gts moans
			}
			ApplyActionCooldown(caster, CooldownSource::Emotion_Moan);
		}
	}

	void Task_SlowGrowTask(Actor* caster, float duration, bool dualCast) {
		std::string name = std::format("SlowGrowTask{}", caster->formID);
		ActorHandle casterhandle = caster->CreateRefHandle();
		double Start = Time::WorldTimeElapsed();

		static Timer timer = Timer(2.33); // Run every 2.33s or as soon as we can
		static Timer deadTimer = Timer(1.5f);
		static Timer MoanTimer = Timer(6.00);

		TaskManager::Run(name, [=](auto& progressData) {
			const auto Caster = casterhandle.get().get();
			if (!Caster) {
				return false;
			}
			const auto ActorTransient = Transient::GetActorData(Caster);
			if (!ActorTransient) {
				return false;
			}
			if (!ActorTransient->IsSlowGrowing) {
				return false;
			}
			if (Caster->IsDead() || GetAV(Caster, ActorValue::kHealth) <= 0.0f) {
				if (deadTimer.ShouldRun()) {
					return false;
				}
			}
			double Finish = Time::WorldTimeElapsed();
			if (Finish - Start >= duration) {
				return false;
			}
			const auto GtsSkillLevel = GetGtsSkillLevel(Caster);
			const float SkillBonus = 1.0f + (GtsSkillLevel * 0.01f); // Calculate bonus power. At the Alteration/Size Mastery of 100 it becomes 200%.

			float power = BASE_POWER * SkillBonus;
			float shakePower = 1.0f;
			float bonus = 1.0f;

			if (timer.ShouldRun()) {
				float Volume = std::clamp(get_visual_scale(Caster) / 8.0f, 0.20f, 1.0f);
				Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundGrowth, Caster, Volume, "NPC Pelvis [Pelv]");
			}

			if (Runtime::HasMagicEffect(Caster, Runtime::MGEF.GTSPotionEffectSizeAmplify)) {
				bonus = get_visual_scale(Caster) * 0.25f + 0.75f;
			}

			if (dualCast) {
				power *= DUAL_CAST_BONUS;
			}
			
			PerformMoanAndParticle(Caster, power, shakePower); // Empowers growth effects

			Rumbling::Once("SlowGrowth", Caster, Rumble_Growth_SlowGrowth_Loop * shakePower, 0.05f, "NPC COM [COM ]", 0.0f, true);
			Grow(Caster, 0.0f, power * bonus);

			return true;
		});
	}
}

namespace GTS {

	std::string SlowGrow::GetName() {
		return "::SlowGrow";
	}

	SlowGrow::SlowGrow(ActiveEffect* effect) : Magic(effect) {

		auto base_spell = GetBaseEffect();

		if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSEffectSlowGrowth)) {
			this->IsDual = false;
		}
		if (base_spell == Runtime::GetMagicEffect(Runtime::MGEF.GTSEffectSlowGrowthDual)) {
			this->IsDual = true;
		}
	}

	void SlowGrow::OnStart() {
		Actor* caster = GetCaster();
		if (caster) {
			if (const auto ActorTransient = Transient::GetActorData(caster)) {
				float scale = get_visual_scale(caster);
				float mult = 0.40f;
				const bool dualCast = this->IsDual || IsDualCasting();
				if (dualCast) {
					Rumbling::Once("SlowGrow", caster, Rumble_Growth_SlowGrowth_Start, 0.10f, "NPC COM [COM ]", 0.0f, true);
					mult *= 1.5f;
				}
				SpawnCustomParticle(caster, ParticleType::Green, NiPoint3(), "NPC COM [COM ]", scale * mult * 1.75f);
				auto duration = this->GetActiveEffect()->duration;
				if (ActorTransient->IsSlowGrowing) {
					ActorTransient->IsSlowGrowing = false;
					return;
 				} else {
					ActorTransient->IsSlowGrowing = true;
					Task_SlowGrowTask(caster, duration, dualCast);
				}
			}
		}
	}

	void SlowGrow::OnFinish() {
		Actor* caster = GetCaster();
		if (caster) {
			if (const auto ActorTransient = Transient::GetActorData(caster)) {
				ActorTransient->IsSlowGrowing = false;
			}
		}
	}
}
