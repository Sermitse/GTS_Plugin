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

    // Main settings
    bool bEnableAutoAim = true;
    bool bDebugAutoAim = false;
    bool bPreventFarStomps = false;
    bool bUseRhombShape = true;

    //------------------------------------------------------------------------------
    //----------------------- hand slam
    float fAimAssist_Range_Hand_Sneak_Slam = 64.0f;
    float fAimAssist_Range_Hand_Sneak_Slam_Strong = 72.0f;
    //----------------------- far stomp
    float fAimAssist_Range_FarStomp = 46.0f;
    float fAimAssist_Range_FarStomp_Strong = 60.0f;
    //----------------------- stomp
    float fAimAssist_Range_Stomp = 32.0f;
    float fAimAssist_Range_Stomp_Sneak = 46.0f;
    float fAimAssist_OffsetDistance_Foot = 10.0f;
    float fAimAssist_OffsetDistance_Foot_FarStomp = 0.0f;
    //----------------------- hand sneak kick
    float fAimAssist_Range_Kick_Sneak = 48.0f;
    float fAimAssist_OffsetDistance_Hand_Sneak_Side = 10.0f;
    float fAimAssist_OffsetDistance_Hand_Sneak_Forward = 0.0f;
    float fAimAssist_OffsetDistance_Hand_Sneak_Forward_Sneak = 50.0f;

    //----------------------- Hand Crawl
    float fAimAssist_Range_Hand_Crawl_Close = 60.0f;
    float fAimAssist_Range_Hand_Crawl_Far = 82.0f;
    float fAimAssist_Range_Hand_Crawl_Strong = 82.0f;
    float fAimAssist_OffsetDistance_Hand_Crawl_Side_Strong = 10.0f;
    float fAimAssist_OffsetDistance_Hand_Crawl_Side_Far = 10.0f;
    float fAimAssist_OffsetDistance_Hand_Crawl_Side = 10.0f;
    float fAimAssist_OffsetDistance_Hand_Crawl_Forward = 0.0f;
    float fAimAssist_OffsetDistance_Hand_Crawl_Forward_Far = 0.0f;
    float fAimAssist_OffsetDistance_Hand_Crawl_Forward_Strong = 0.0f;

    //----------------------- kick
    float fAimAssist_Range_Kick = 48.0f;
    float fAimAssist_OffsetDistance_Kick_Forward = 40.0f;
    //----------------------- butt
    float fAimAssist_Range_ButtSlam = 48.0f;
    float fAimAssist_OffsetDistance_Butt_Forward = 0.0f;
    float fAimAssist_OffsetDistance_Butt_Side = 15.0f;

    //----------------------- breast
    float fAimAssist_Range_BreastSlam = 30.0f;
    float fAimAssist_OffsetDistance_Breast_Forward = 15.0f;
    float fAimAssist_OffsetDistance_Breast_Side = 4.0f;

    //----------------------- Auto-Aim settings
    float fAimAssist_BackPenalty = 30.0f;
    float fAimAssist_DeadPenalty = 50.0f;
    float fAimAssist_IgnoreBehindAfter = 0.25f;
    float fAimAssist_AimMagnitudeMultiplier = 1.0f;
    float fAimAssist_NoHitValueRandomRange = 0.25f;

};
TOML_SERIALIZABLE(SettingsAutoAim_t);
TOML_REGISTER_NAME(SettingsAutoAim_t, "AutoAim");