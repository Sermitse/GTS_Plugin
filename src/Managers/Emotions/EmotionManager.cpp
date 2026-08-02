#include "Managers/Emotions/EmotionManager.hpp"
#include "Managers/Animation/AnimationManager.hpp"

using namespace RE;
using namespace GTS;

namespace {

	constexpr float Speed_up = 12.0f;

	BSFaceGenAnimationData* GetFacialData(Actor* giant) {
		auto fgen = giant->GetFaceGenAnimationData();
		if (fgen) {
			return fgen;
		}
		return nullptr;
	}

	float Phenome_GetPhenomeValue(BSFaceGenAnimationData* data, std::uint32_t Phenome) {
		return data->phenomeKeyFrame.values[Phenome];
	}

	float Phenome_GetModifierValue(BSFaceGenAnimationData* data, std::uint32_t Modifier) {
		return data->modifierKeyFrame.values[Modifier];
	}

	void Phenome_ManagePhenomes(BSFaceGenAnimationData* data, std::uint32_t Phenome, float Value) {
		data->phenomeKeyFrame.SetValue(Phenome, Value);
	}

	void Phenome_ManageModifiers(BSFaceGenAnimationData* data, std::uint32_t Modifier, float Value) {
		data->modifierKeyFrame.SetValue(Modifier, Value);
	}

	// Deterministic per-(actor, index) task name - re-triggering the same
	// slot resolves to the same task, so there's only ever one tick loop per
	// index (see the retarget logic in Task_UpdatePhenome/Task_UpdateModifier).
	std::string PhenomeTaskName(Actor* giant, int phenome) {
		return std::format("Phenome_{}_{}", giant->formID, phenome);
	}

	std::string ModifierTaskName(Actor* giant, int modifier) {
		return std::format("Modifier_{}_{}", giant->formID, modifier);
	}

	struct RampResult {
		float value;
		bool  finished;
	};

	// Walks `value` from ramp.StartValue toward ramp.Target at `unitsPerSecond`,
	// clamped so it can never overshoot. Shared by both the Phenome and
	// Modifier tick loops.
	RampResult AdvanceRamp(const EmotionRampState& ramp, double now, float unitsPerSecond) {
		float direction = ramp.Target - ramp.StartValue;
		if (std::fabs(direction) <= 1e-4f) {
			return { ramp.Target, true };
		}

		double pass = now - ramp.StartTime;
		float moved = static_cast<float>(pass) * unitsPerSecond;
		float sign = direction > 0.0f ? 1.0f : -1.0f;

		if (moved >= std::fabs(direction)) {
			return { ramp.Target, true };
		}
		return { ramp.StartValue + sign * moved, false };
	}

	void Task_UpdatePhenome(Actor* giant, int phenome, float mfg_speed, float target) {
		auto data = Transient::GetActorData(giant);
		if (!data) {
			return;
		}

		float currentValue = EmotionManager::GetEmotionValue(giant, CharEmotionType::Phenome, phenome);

		// (Re)target this phenome's ramp. If a task is already ticking this
		// exact index, it reads this every frame (see below) and will just
		// bend towards the new target starting from wherever the value
		// actually is right now - no snap, no restart needed.
		data->PhenomeRamps[phenome] = EmotionRampState{ Time::WorldTimeElapsed(), currentValue, target, mfg_speed };

		if (EmotionManager::IsEmotionBusy(giant, CharEmotionType::Phenome, phenome)) {
			return; // the already-running task above will pick up the retarget on its next tick
		}

		std::string name = PhenomeTaskName(giant, phenome);
		ActorHandle giantHandle = giant->CreateRefHandle();

		EmotionManager::SetEmotionBusy(giant, CharEmotionType::Phenome, phenome, true);

		TaskManager::Run(name, [=](auto& progressData) {
			if (!giantHandle) {
				return false;
			}
			auto giantref = giantHandle.get().get();

			if (!giantref->Is3DLoaded()) {
				// FIX: the old version returned here without clearing the busy
				// flag, leaving this phenome permanently locked as "busy" if
				// the actor's 3D unloaded mid-ramp (e.g. a cell change).
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, phenome, false);
				return false;
			}

			auto FaceData = GetFacialData(giantref);
			auto transient = Transient::GetActorData(giantref);
			if (!FaceData || !transient) {
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, phenome, false);
				return false;
			}

			// Re-read the ramp every tick (rather than capturing it once) -
			// this is what lets a later call for this same index redirect an
			// already-running ramp instead of being ignored or snapping.
			const EmotionRampState& ramp = transient->PhenomeRamps[phenome];

			float AnimSpeed = AnimationManager::GetSingleton().GetAnimSpeed(giantref);
			float speed = 1.25f * AnimSpeed * ramp.MfgSpeed * Speed_up;

			auto result = AdvanceRamp(ramp, Time::WorldTimeElapsed(), speed);
			Phenome_ManagePhenomes(FaceData, phenome, result.value);

			if (result.finished) {
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, phenome, false);
				return false;
			}
			return true;
		});
	}

	void Task_UpdateModifier(Actor* giant, int modifier, float mfg_speed, float target) {
		auto data = Transient::GetActorData(giant);
		if (!data) {
			return;
		}

		float currentValue = EmotionManager::GetEmotionValue(giant, CharEmotionType::Modifier, modifier);
		data->ModifierRamps[modifier] = EmotionRampState{ Time::WorldTimeElapsed(), currentValue, target, mfg_speed };

		if (EmotionManager::IsEmotionBusy(giant, CharEmotionType::Modifier, modifier)) {
			return;
		}

		std::string name = ModifierTaskName(giant, modifier);
		ActorHandle giantHandle = giant->CreateRefHandle();

		EmotionManager::SetEmotionBusy(giant, CharEmotionType::Modifier, modifier, true);

		TaskManager::Run(name, [=](auto& progressData) {
			if (!giantHandle) {
				return false;
			}
			auto giantref = giantHandle.get().get();

			if (!giantref->Is3DLoaded()) {
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Modifier, modifier, false);
				return false;
			}

			auto FaceData = GetFacialData(giantref);
			auto transient = Transient::GetActorData(giantref);
			if (!FaceData || !transient) {
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Modifier, modifier, false);
				return false;
			}

			const EmotionRampState& ramp = transient->ModifierRamps[modifier];

			float AnimSpeed = AnimationManager::GetSingleton().GetAnimSpeed(giantref);
			float speed = 1.0f * AnimSpeed * ramp.MfgSpeed * Speed_up;

			auto result = AdvanceRamp(ramp, Time::WorldTimeElapsed(), speed);
			Phenome_ManageModifiers(FaceData, modifier, result.value);

			if (result.finished) {
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Modifier, modifier, false);
				return false;
			}
			return true;
		});
	}
}

namespace GTS {

	void EmotionManager::SetEmotionBusy(Actor* giant, CharEmotionType Type, std::uint32_t index, bool lock) {
		auto data = Transient::GetActorData(giant);
		if (!data) {
			return;
		}
		switch (Type) {
			case CharEmotionType::Modifier:
				if (lock) {
					data->BusyEmotionModifiers.insert(index);
				} else {
					data->BusyEmotionModifiers.erase(index);
				}
			break;
			case CharEmotionType::Phenome:
				if (lock) {
					data->BusyEmotionPhenomes.insert(index);
				} else {
					data->BusyEmotionPhenomes.erase(index);
				}
			break;
			default:
				break; // Expression is applied instantly (no ramp task), so it's never tracked as "busy"
		}
	}

	bool EmotionManager::IsEmotionBusy(Actor* giant, CharEmotionType Type, std::uint32_t index) {
		auto data = Transient::GetActorData(giant);
		if (!data) {
			return false;
		}
		switch (Type) {
			case CharEmotionType::Modifier:
				return data->BusyEmotionModifiers.contains(index);
			case CharEmotionType::Phenome:
				return data->BusyEmotionPhenomes.contains(index);
			default:
				return false;
		}
	}

	float EmotionManager::GetEmotionValue(Actor* giant, CharEmotionType Type, std::uint32_t emotion_value) {
		float value = 0.0f;
		auto data = GetFacialData(giant);
		if (data) {
			switch (Type) {
				case CharEmotionType::Modifier:
					value = Phenome_GetModifierValue(data, emotion_value);
				break;
				case CharEmotionType::Phenome:
					value = Phenome_GetPhenomeValue(data, emotion_value);
				break;
				default:
				break;
			}
		}
		return value;
	}

	void EmotionManager::OverridePhenome(Actor* giant, int number, float mfg_speed, float target) {
		Task_UpdatePhenome(giant, number, mfg_speed, target);
	}

	void EmotionManager::OverrideModifier(Actor* giant, int number, float mfg_speed, float target) {
		Task_UpdateModifier(giant, number, mfg_speed, target);
	}
}