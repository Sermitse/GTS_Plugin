#pragma once

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

enum class LSizeMode_t : uint8_t {
    kNormal,
    kMassBased
};

//-------------------------------------------------------------------------------------------------------------------
//  BASE STRUCT
//  (Directly Serialized)
//-------------------------------------------------------------------------------------------------------------------

struct SettingsBalance_t {

    // General balance parameters
    std::string sSizeMode = "kNormal";
    float fSpellEfficiency = 0.55f;
    float fSizeDamageMult = 1.0f;
    float fExpMult = 1.0f;
    float fStatBonusDamageMult = 1.0f;
    float fStatBonusCarryWeightMult = 1.0f;

    // Friendly interaction settings
    bool bPlayerFriendlyImmunity = false;
    bool bFollowerFriendlyImmunity = false;
    bool bAllowFriendlyStagger = true;
    bool bAllowOthersStagger = true;

    // Size limits
    float fMaxPlayerSizeOverride = 0.0f;
    float fMaxFollowerSize = 0.0f;
    float fMaxOtherSize = 0.0f;


    // Balance mode adjustments
    bool bBalanceMode = false;
    float fBMSizeGainPenaltyMult = 1.0f;
    float fBMShrinkRate = 1.0f;
    float fBMShrinkRateCombat = 0.08f;
    float fBMShrinkOnHitMult = 1.0f;

};
TOML_SERIALIZABLE(SettingsBalance_t);
TOML_REGISTER_NAME(SettingsBalance_t, "Balance");