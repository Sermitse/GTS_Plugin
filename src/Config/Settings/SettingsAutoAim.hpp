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
//  ENUMS ----- Assumed to be the reference values 
//  magic_enum will use to convert an enum to a string representation for serialization (Saving The TOML)
//-------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------
//  BASE STRUCT
//  (Directly Serialized)
//-------------------------------------------------------------------------------------------------------------------

struct SettingsAutoAim_t {

    // Friendly interaction settings
    bool bEnableAutoAim = true;
    bool bDebugAutoAim = false;

    // Collider ranges
    float fAutoAim_Range_Stomp = 36.5f;
    float fAutoAim_Range_Hand = 15.0f;
    float fAutoAim_Range_Kick = 36.0f;

    // Initial Offsets
    float fAutoAim_Foot_OffsetDistance = 10.0f;
    float fAutoAim_Hand_OffsetDistance_Side = 14.5f;
    float fAutoAim_Hand_OffsetDistance_Forward = 50.0f;
    float fAutoAim_Hand_OffsetDistance_Forward_Sneak = 35.0f;
    float fAutoAim_Kick_OffsetDistance_Forward = 20.0f;

    // Auto-Aim settings
    float fAutoAim_BackPenalty = 10.0f;
    float fAutoAim_DeadPenalty = 20.0f;
    float fAutoAim_IgnoreBehindAfter = 0.25f;
    float fAutoAim_aimMagnitudeMultiplier = 1.25f;
    float fAutoAim_noHitValueRandomRange = 0.25f;

};
TOML_SERIALIZABLE(SettingsAutoAim_t);
TOML_REGISTER_NAME(SettingsAutoAim_t, "AutoAim");