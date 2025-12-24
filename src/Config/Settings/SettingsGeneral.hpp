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

struct SettingsGeneral_t {

    // Visual settings
    bool bLessGore = false;
    bool bShowHearts = true;
    bool bShowIcons = true;

    // Compatibility options
    bool bDevourmentCompat = false;
    bool bConversationCamCompat = false;
    bool bOverrideInteractionDist = false;
    bool bAlternativeSpeedFormula = true;

    // Protection toggles
    bool bProtectEssentials = true;
    bool bProtectFollowers = true;

    // Gameplay enhancements
    bool bDynamicSizePlayer = false;
    bool bDynamicSizeFollowers = false;

    bool bDynamicFurnSizePlayer = false;
    bool bDynamicFurnSizeFollowers = false;

    bool bDynamicAnimspeed = true;
    bool bEnableHighHeels = true;
    bool bHighheelsFurniture = true;
    bool bEnableMales = false;
    bool bAllActorSizeEffects = false;
    bool bEnableFOVEdits = true;

    // Bone tracking for foot Animations
    bool bTrackBonesDuringAnim = true;

    bool bPlayerLootpiles = true;
    bool bFollowerLootpiles = true;

    bool bAlterPlayerGravity = false;
    float fAdditionalJumpEffectDelay = 0.15f;
    float fAdditionalJumpEffectDelay_Gravity = 0.0f;

	float fNPCMaxSpeedMultClampTarget = 1.0f;  //Max speed multiplier target at max clamp for the follow procedure
    float fNPCMaxSpeedMultClampTargetCombat = 0.5f;  //Max speed multiplier target at max clamp for the follow procedure
	float fNPCMaxSpeedMultClampStartAt = 2.5f; //Scale at which clamping begins
	float fNPCMaxSpeedMultClampMaxAt = 3.5f;   //Scale at which speed is fully clamped to target
};
TOML_SERIALIZABLE(SettingsGeneral_t);
TOML_REGISTER_NAME(SettingsGeneral_t, "General");