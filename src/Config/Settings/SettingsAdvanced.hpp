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

struct SettingsAdvanced_t {

    // Logging levels
    std::string sLogLevel = "err";

    // Toggles for advanced features
    bool bShowOverlay = false;
    bool bDamageAV = true;
    bool bCooldowns = true;
    bool bEnforceUIClamps = true;
    bool bPlayerAI = false;
    float fAnimSpeedAdjMultPlayer = 1.0f;
    float fAnimSpeedAdjMultTeammate = 1.0f;
    bool bEnableExperimentalDevourmentAI = false;
    float fExperimentalDevourmentAIProb = 25.0f;
    std::array<float, 5> fAnimSpeedFormula = { 0.142f, 0.82f, 1.90f, 1.0f, 0.0f };
    bool bGTSAnimsFullSpeed = false;
    float fAnimspeedLowestBoundAllowed = 0.01f;

};
TOML_SERIALIZABLE(SettingsAdvanced_t);
TOML_REGISTER_NAME(SettingsAdvanced_t, "Advanced");
