#pragma once
#include "Config/Util/TomlRefl.hpp"

/*
Reflection based serializer limitations:

 - C style arrays are unsupported so no "char foo[32]" for example
 - std::array arrays or std::vectors however do work with basic types (aggregate types are untested)
 - Data types like std::tuple or ordered map are unsupported (others are untested)
 - Nested structs are supported as long as these also are put through the TOML_SERIALIZABLE() macro.
 - Structs can only contain a total of 64 unique entries, this is a limitation of the reflect library.
   In order to be able to have > 64 the visit template in the reflect library needs to be expanded.
 - Enums are also unsupported. They can however be saved as either int or string. Its better to save them as a string though
   and use something like magic_enum to do the conversion between string <-> enum.
*/

/* Naming Convention
* i(x) -> integer(Ammount of elements if array)
* f(x) -> float(Ammount of elements if array)
* b(x) -> bool(Ammount of elements if array)
* s(x) -> string(Ammount of elements if array)
* stucts don't get a special notation
*/

//-------------------------------------------------------------------------------------------------------------------
//  BASE STRUCT
//  (Directly Serialized)
//-------------------------------------------------------------------------------------------------------------------

struct SettingsAudio_t {

    // Footstep sounds
    bool bFootstepSounds = true;
    bool bUseOldSounds = false;

    // Voice settings
    bool bSlowGrowMoans = true;
    bool bEnableVoicePitchOverrideN = true;
    bool bEnableVoicePitchOverrideG = false;
    bool bEnableGorePitchOverride = false;
    bool bMuteVoreDeathScreams = true;
    bool bMuteHugCrushDeathScreams = true;
    bool bMuteFingerSnapDeathScreams = true;
    bool bMuteBreastAbsorptionDeathScreams = true;
    bool bMuteShrinkToNothingDeathScreams = false;
    bool bMuteCrushDeathScreams = false;


    bool bMoanLaughSizeVariants = true;
    bool bLaughEnable = true;
    bool bMoanEnable = true;
    bool bMoanLaughPCExclusive = false;
    bool bUseOtherHighHeelSet = true;
    bool bBlendBetweenFootsteps = false;

    float fMaxVoiceFreq = 1.6f;                // > Higher Value means Higher Freq -> Higher Voice
    float fMinVoiceFreq = 0.8355f;             // > Lower Value means Lower Freq -> Lower Voice

    float fTargetPitchAtScaleMax = 8.0f;
    float fTargetPitchAtScaleMin = 0.2f;

    float fFallOffMultiplier = 1.0f;
    float fVoiceVolumeMult = 1.0f;

};
TOML_SERIALIZABLE(SettingsAudio_t);
TOML_REGISTER_NAME(SettingsAudio_t, "Audio");