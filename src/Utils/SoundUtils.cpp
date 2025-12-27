#include "Utils/SoundUtils.hpp"
#include "Config/Config.hpp"
#include "Managers/Audio/Footstep.hpp"

namespace GTS {
	
	void DoFootstepSound(Actor* giant, float modifier, FootEvent kind, std::string_view node) {
		auto& footstepSound = FootStepManager::GetSingleton();

		std::vector<NiAVObject*> points = { find_node(giant, node) };

		Impact impact_data = Impact{
			.actor = giant,
			.kind = kind,
			.scale = get_visual_scale(giant),
			.modifier = modifier,
			.nodes = points,
		};
		footstepSound.OnImpact(impact_data); // Play sound
	}

	float Sound_GetFallOff(NiAVObject* source, float mult) {
		if (source) {
			float distance_to_camera = GameUnitToMeter(get_distance_to_camera(source));
			// Camera distance based volume falloff
			return soft_core(distance_to_camera, 0.024f / mult, 2.0f, 0.8f, 0.0f, 0.0f);
		}
		return 1.0f;
	}

	std::tuple<float, float> CalculateVoicePitch(Actor* a_actor) {

		static auto& Audio = Config::Audio;
		const float& MinFreq = Audio.fMinVoiceFreq;            // e.g. 0.2
		const float& MaxFreq = Audio.fMaxVoiceFreq;            // e.g. 2.0
		const float& ScaleMax = Audio.fTargetPitchAtScaleMax;  // e.g. 50.0 (scale at which MinFreq is reached)
		const float& ScaleMin = Audio.fTargetPitchAtScaleMin;  // e.g. 0.2  (scale at which MaxFreq is reached)

		const float scale = get_corrected_scale(a_actor);

		// volume = clamped [0.35,1.0] based on scale+0.5
		const float volume = std::clamp(scale + 0.5f, 0.35f, 1.0f);

		float freq = 1.0f;
		if (scale > 1.0f) {
			// Decrease linearly from 1.0 MinFreq as scale goes 1.0 ScaleMax
			float t = (scale - 1.0f) / (ScaleMax - 1.0f);
			t = std::clamp(t, 0.0f, 1.0f);
			freq = std::lerp(1.0f, MinFreq, t);
			//logger::trace("ComputedFreq > 1.0f: {}", freq);
		}
		else if (scale < 1.0f) {
			// Increase linearly from 1.0 MaxFreq as scale goes 1.0 ScaleMin
			float t = (1.0f - scale) / (1.0f - ScaleMin);
			t = std::clamp(t, 0.0f, 1.0f);
			freq = std::lerp(1.0f, MaxFreq, t);
			//logger::trace("ComputedFreq < 1.0f: {}", freq);
		}

		return { volume, freq };
	}

	float CalculateGorePitch(float TinyScale) {
		const bool bAlterGorePitch = Config::Audio.bEnableGorePitchOverride;
		float freq = 1.0f;

		if (bAlterGorePitch && TinyScale < 1.0f) {
			float t = (1.0f - TinyScale) / (1.0f - 0.2f);
			t = std::clamp(t, 0.0f, 1.0f);
			freq = std::lerp(1.0f, 2.0f, t);
		}

		return freq;
	}

}
