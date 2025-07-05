#include "Magic/Effects/Slow_Grow.hpp"
#include "Magic/Effects/Common.hpp"

#include "Managers/Rumble.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"

#include "Managers/Audio/MoansLaughs.hpp"

using namespace GTS;

namespace {

	constexpr float BASE_POWER = 0.000025f; // Default growth over time.
	constexpr float DUAL_CAST_BONUS = 2.25f;

	bool PerformMoanAndParticle(Actor* caster) {
		if (caster && IsFemale(caster) && !IsActionOnCooldown(caster, CooldownSource::Emotion_Moan)) {
			for (auto Foot: {"NPC L Foot [Lft ]", "NPC R Foot [Rft ]"}) {
				auto FootNode = find_node(caster, Foot);
				if (FootNode) {
					SpawnCustomParticle(caster, ParticleType::Green, FootNode->world.translate, Foot, 0.75f);
				}
			}

			if (Config::GetAudio().bSlowGrowMoans) {
				Task_FacialEmotionTask_Moan(caster, 1.0f + RandomFloat(0.0f, 0.25f), "SlowGrow");
				float MoanVolume = std::clamp(get_visual_scale(caster)/8.0f, 0.25f, 1.0f);
				Sound_PlayMoans(caster, MoanVolume, 0.14f, EmotionTriggerSource::Growth);
			}
			ApplyActionCooldown(caster, CooldownSource::Emotion_Moan);
			return true;
		}
		return false;
	}
}

namespace GTS {

	void SlowGrow::Task_SlowGrowTask(Actor* caster) {

		std::string name = std::format("SlowGrowTask{}", caster->formID);
		ActorHandle casterhandle = caster->CreateRefHandle();

		TaskManager::Run(name, [=,this](auto& progressData) {

			const auto CasterActor = casterhandle.get().get();

			if (!CasterActor) {
				return false;
			}

			const auto ActorTransient = Transient::GetSingleton().GetData(CasterActor);
			if (!ActorTransient) {
				return false;
			}

			if (!ActorTransient->IsSlowGrowing) {
				return false;
			}

			const auto GtsSkillLevel = GetGtsSkillLevel(CasterActor);
			const float SkillBonus = 1.0f + (GtsSkillLevel * 0.01f); // Calculate bonus power. At the Alteration/Size Mastery of 100 it becomes 200%.

			float power = BASE_POWER * SkillBonus;
			float bonus = 1.0f;

			if (timer.ShouldRun()) {
				float Volume = std::clamp(get_visual_scale(CasterActor) / 8.0f, 0.20f, 1.0f);
				Runtime::PlaySoundAtNode("GTSSoundGrowth", CasterActor, Volume, 1.0f, "NPC Pelvis [Pelv]");
			}

			if (Runtime::HasMagicEffect(CasterActor, "GTSPotionEffectSizeAmplify")) {
				bonus = get_visual_scale(CasterActor) * 0.25f + 0.75f;
			}

			if (IsDual) {
				power *= DUAL_CAST_BONUS;
			}

			PerformMoanAndParticle(CasterActor) ? power *= 320.0f : power *= 0.625f;
			// Mini growth spurts, else weaker growth over time

			Rumbling::Once("SlowGrow", CasterActor, Rumble_Growth_SlowGrowth_Loop, 0.05f);
			Grow(CasterActor, 0.0f, power * bonus);

			return true;
		});
	}

	std::string SlowGrow::GetName() {
		return "::SlowGrow";
	}

	SlowGrow::SlowGrow(ActiveEffect* effect) : Magic(effect) {

		auto base_spell = GetBaseEffect();

		if (base_spell == Runtime::GetMagicEffect("GTSEffectSlowGrowth")) {
			this->IsDual = false;
		}
		if (base_spell == Runtime::GetMagicEffect("GTSEffectSlowGrowthDual")) {
			this->IsDual = true;
		}
	}

	void SlowGrow::OnStart() {
		Actor* caster = GetCaster();
		if (caster) {

			const auto ActorTransient = Transient::GetSingleton().GetData(caster);
			if (ActorTransient) {

				float scale = get_visual_scale(caster);
				float mult = 0.40f;
				if (this->IsDual) {
					Rumbling::For("SlowGrow", caster, Rumble_Growth_SlowGrowth_Start, 0.10f, "NPC COM [COM ]", 0.35f, 0.0f);
					mult *= 1.5f;
				}
				SpawnCustomParticle(caster, ParticleType::Green, NiPoint3(), "NPC COM [COM ]", scale * mult * 1.75f);

				if (!ActorTransient->IsSlowGrowing) {
					ActorTransient->IsSlowGrowing = true;
					Task_SlowGrowTask(caster);
				}
				else {
					ActorTransient->IsSlowGrowing = false;
				}
			}
		}
	}

	void SlowGrow::OnUpdate() {}

	void SlowGrow::OnFinish() {}
}
