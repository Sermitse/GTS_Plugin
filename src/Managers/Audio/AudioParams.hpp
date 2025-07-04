#pragma once
// Module that handles footsteps

namespace GTS {
	struct VolumeParams {
        float a;
        float k;
        float n;
        float s;
    };
	struct StepSoundData {
		float limit;
		int soundLevel;
		const VolumeParams& paramsStart;
		const VolumeParams& paramsEnd;
		std::string label;
		float volume;
		bool blend;
		float extra_volume;
	};

    // Params
	constexpr VolumeParams Params_Empty = {.a = 0.0f, .k = 0.0f, .n = 0.0f, .s = 0.0f};

	constexpr VolumeParams xlFootstep_Params = {.a = 12.0f, .k = 0.50f, .n = 0.5f, .s = 1.0f};
	constexpr VolumeParams xxlFootstep_Params = {.a = 20.0f, .k = 0.50f,  .n = 0.5f, .s = 1.0f};
	constexpr VolumeParams lJumpLand_Params = {.a = 1.2f, .k = 0.65f,  .n = 0.7f, .s = 1.0f};

	constexpr VolumeParams xlRumble_Params = {.a = 12.0f, .k = 0.50f, .n = 0.5f, .s = 1.0f};

	constexpr VolumeParams BL_Footstep_1_5_Params = {.a = 1.2f, .k = 1.0f, .n = 0.9f, .s = 3.35f};    // https://www.desmos.com/calculator/6yzmmrg3oi
	constexpr VolumeParams BL_Footstep_2_Params = {.a = 1.65f, .k = 1.0f, .n = 0.75f, .s = 1.0f};    // https://www.desmos.com/calculator/6yzmmrg3oi
	constexpr VolumeParams BL_Footstep_4_Params = {.a = 3.0f, .k = 1.0f, .n = 1.15f, .s = 1.0f};     // https://www.desmos.com/calculator/ighurmlanl
	constexpr VolumeParams BL_Footstep_8_Params = {.a = 6.0f, .k = 0.26f, .n = 1.94f, .s = 1.0f};    // https://www.desmos.com/calculator/d3gsgj6ocs
	constexpr VolumeParams BL_Footstep_12_Params = {.a = 12.0f, .k = 0.27f, .n = 1.9f, .s = 1.0f};   // https://www.desmos.com/calculator/akoyl4cxch
	constexpr VolumeParams BL_Footstep_24_Params = {.a = 16.0f, .k = 0.36f, .n = 1.5f, .s = 0.25f};  // https://www.desmos.com/calculator/bh4fhfrji6
	constexpr VolumeParams BL_Footstep_48_Params = {.a = 36.0f, .k = 0.20f, .n = 1.5f, .s = 0.25f};  // https://www.desmos.com/calculator/3q4qgkrker
	constexpr VolumeParams BL_Footstep_96_Params = {.a = 80.0f, .k = 0.66f, .n = 0.90f, .s = 0.1f};  // https://www.desmos.com/calculator/ufdbieymdi
	constexpr VolumeParams BL_Footstep_128_Params = {.a = 124.0f, .k = 1.0f, .n = 1.15f, .s = 1.0f}; //{.a = 118.0f, .k = 0.19f, .n = 1.5f, .s = 0.1f}; // https://www.desmos.com/calculator/pvrge5dejz
	 // These are used for blending

	constexpr VolumeParams Footstep_1_5_Params = {.a = 1.2f, .k = 1.0f, .n = 0.9f, .s = 2.0f};    // https://www.desmos.com/calculator/6yzmmrg3oi
	constexpr VolumeParams Footstep_2a_Params = {.a = 1.25f, .k = 1.0f, .n = 0.9f, .s = 1.25f};// https://www.desmos.com/calculator/6yzmmrg3oi
	constexpr VolumeParams Footstep_2_Params = {.a = 2.0f, .k = 1.0f, .n = 0.75f, .s = 1.0f};    // https://www.desmos.com/calculator/6yzmmrg3oi
	constexpr VolumeParams Footstep_4_Params = {.a = 4.0f, .k = 1.0f, .n = 1.15f, .s = 1.0f};     // https://www.desmos.com/calculator/ighurmlanl
	constexpr VolumeParams Footstep_8_Params = {.a = 8.0f, .k = 0.26f, .n = 1.94f, .s = 1.0f};    // https://www.desmos.com/calculator/d3gsgj6ocs
	constexpr VolumeParams Footstep_12_Params = {.a = 14.0f, .k = 0.27f, .n = 1.9f, .s = 1.0f};   // https://www.desmos.com/calculator/akoyl4cxch
	constexpr VolumeParams Footstep_24_Params = {.a = 24.0f, .k = 0.36f, .n = 1.5f, .s = 0.25f};  // https://www.desmos.com/calculator/bh4fhfrji6
	constexpr VolumeParams Footstep_48_Params = {.a = 48.0f, .k = 0.20f, .n = 1.5f, .s = 0.25f};  // https://www.desmos.com/calculator/3q4qgkrker
	constexpr VolumeParams Footstep_96_Params = {.a = 96.0f, .k = 0.66f, .n = 0.90f, .s = 0.1f};  // https://www.desmos.com/calculator/ufdbieymdi
	constexpr VolumeParams Footstep_128_Params = {.a = 128.0f, .k = 1.0f, .n = 1.15f, .s = 1.0f}; //{.a = 118.0f, .k = 0.19f, .n = 1.5f, .s = 0.1f}; // https://www.desmos.com/calculator/pvrge5dejz
	// Params end

	constexpr float limitless = 0.0f;
	constexpr float limit_x2 = 2.0f;
	constexpr float limit_x4 = 4.0f;
	constexpr float limit_x8 = 8.0f;
	constexpr float limit_x12 = 12.0f;
	constexpr float limit_x14 = 14.0f;
	constexpr float limit_x24 = 24.0f;
	constexpr float limit_x48 = 48.0f;
	constexpr float limit_x96 = 96.0f;
	constexpr float limit_mega = 128.0f;
}