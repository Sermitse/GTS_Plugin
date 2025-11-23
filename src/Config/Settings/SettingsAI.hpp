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
//  CHILD STRUCTS 
//  (Not Directly Serialized, but used within other structs)
//-------------------------------------------------------------------------------------------------------------------

// Stateless actions: simple probability-based triggers.
struct AIStatelessAction_t {
    bool bEnableAction = true;
    float fProbability = 50.0f;
};
TOML_SERIALIZABLE(AIStatelessAction_t);

// Statefull actions: have additional probability settings and an interval.
struct AIStatefullAction_t {
    bool bEnableAction      = true;
    float fProbability      = 50.0f;
    float fProbabilityLight = 50.0f;
    float fProbabilityHeavy = 50.0f;
    float fInterval         = 2.0f;
};
TOML_SERIALIZABLE(AIStatefullAction_t);

// Complex action: Hug
struct AIHugAction_t {
    // Basic toggle and overall probability
    bool bEnableAction = true;
    float fProbability = 35.0f;

    // Conditions
    bool bStopIfCantShrink      = true;
    bool bKillFriendlies        = true;
    bool bKillFollowersOrPlayer = false;

    // Outcome probabilities
    float fHealProb           = 30.0f;
    float fShrinkProb         = 100.0f;
    float fFriendlyShrinkProb = 15.0f;
    float fKillProb           = 30.0f;

    // Timing
    float fInterval = 1.5f;
};
TOML_SERIALIZABLE(AIHugAction_t);

struct AIStompAction_t {
    bool bEnableAction                = true;
    float fProbability                = 33.0f;
    float fUnderstompGrindProbability = 40.0f;
};
TOML_SERIALIZABLE(AIStompAction_t);

// Complex action: Butt Crush
struct AIButtAction_t {
    bool bEnableAction = true;
    float fProbability = 50.0f;

    // Different probabilities for variations of the action
    float fFastProb          = 50.0f;
    float fButtCrushTypeProb = 50.0f;
    float fGrowProb          = 35.0f;
    float fCrushProb         = 35.0f;

    // Timing and limit
    float fInterval = 2.0f;
};
TOML_SERIALIZABLE(AIButtAction_t);

// Complex action: Grab
struct AIGrabAction_t {
    bool bEnableAction = true;
    float fProbability = 33.0f;

    // Different attack types
    float fThrowProb   = 33.0f;
    float fVoreProb    = 50.0f;
    float fCrushProb   = 50.0f;
    float fReleaseProb = 0.0f;

    // Cleavage (multiple stages) probabilities
    float fCleavageProb          = 65.0f;
    float fCleavageAttackProb    = 35.0f;
    float fCleavageAbsorbProb    = 35.0f;
    float fCleavageVoreProb      = 35.0f;
    float fCleavageSuffocateProb = 35.0f;
    float fCleavageStopProb      = 0.0f;
    float fStrangleChance        = 35.0f;

    // Cleavage (multiple stages) probabilities
    float fGrabPlayStartProb      = 65.0f;
    float fGrabPlayHeavyCrushProb = 35.0f;
    float fGrabPlayVoreProb       = 35.0f;
    float fGrabPlayKissProb       = 35.0f;
    float fGrabPlayKissVoreProb   = 15.0f;
    float fGrabPlayPokeProb       = 35.0f;
    float fGrabPlayFlickProb      = 35.0f;
    float fGrabPlaySandwichProb   = 35.0f;
    float fGrabPlayGrindStartProb = 35.0f;
    float fGrabPlayGrindStopProb  = 5.0f;
    float fGrabPlayExitProb       = 10.0f;

    // Timing
    float fInterval = 1.5f;

};
TOML_SERIALIZABLE(AIGrabAction_t);

//-------------------------------------------------------------------------------------------------------------------
//  BASE STRUCT
//  (Directly Serialized)
//-------------------------------------------------------------------------------------------------------------------

struct SettingsAI_t {
    // Global AI settings
    bool bEnableActionAI = true;
    float fMasterTimer   = 3.0f;

    // Stateless Actions
    AIStatelessAction_t Vore      = { .bEnableAction = true, .fProbability = 33.0f };
    AIStatelessAction_t KickSwipe = { .bEnableAction = true, .fProbability = 25.0f };

    // Statefull Actions
    AIStatefullAction_t ThighCrush    = { .bEnableAction = true, .fProbability = 15.0f, .fInterval = 1.0f };
    AIStatefullAction_t ThighSandwich = { .bEnableAction = true, .fProbability = 20.0f, .fInterval = 1.5f };

    // Complex Actions
    AIStompAction_t Stomp    = {};
    AIHugAction_t Hugs       = {};
    AIButtAction_t ButtCrush = {};
    AIGrabAction_t Grab      = {};

    // Additional AI toggles
    bool bPanic                = true;
    bool bCombatOnly           = true;
    bool bAllowPlayer          = true;
    bool bAllowFollowers       = true;
    bool bHostileOnly          = true;
    bool bDisableAttacks       = true;
    bool bAlwaysDisableAttacks = false;
    bool bSlowMovementDown     = true;
    bool bSlowRotationDown     = true;
    bool bRecordBoneSpeedData  = false;
};
TOML_SERIALIZABLE(SettingsAI_t);
TOML_REGISTER_NAME(SettingsAI_t, "AI");

