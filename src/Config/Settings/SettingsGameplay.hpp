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

enum class LActiveGamemode_t : uint8_t {
    kNone,
    kGrow,
    kShrink,
    kCombatGrowth,
    kSlowCombatGrowth,
    kCurseOfGrowth,
    kCurseOfTheGiantess,
    kCurseOfDiminishing,
    kSizeLocked,
    kLevelLocked
};

//-------------------------------------------------------------------------------------------------------------------
//  CHILD STRUCTS 
//  (Not Directly Serialized, but used within other structs)
//-------------------------------------------------------------------------------------------------------------------

//16 ought to be enough
struct GameplayMorphSettings_t {
    float fMultiplier = 1.0f;
    std::array<std::string, 16> MorphNames;
    std::array<float, 16> MorphScales;
};
TOML_SERIALIZABLE(GameplayMorphSettings_t);

struct GameplayActionSettings_t {

    // Animation options
    bool bStompAlternative = false;
    bool bStomAlternativeOther = false;
    bool bSneakTransitions = true;
    bool bSneakTransitionsOther = true;

    // Vore-related settings
    float fVoreGainMult = 1.0f;
    bool bVoreFreecam = false;
    bool bVoreWeightGain = false;
    bool bAllowInsects = false;
    bool bAllowUndead = false;
    bool bDVDoEndoOnTeam = false;
    bool bEnableBellyMorph = true;
    float fBellyAbsorbIncrementBy = 0.55f;

    //Stomp Settings
    float fPlayerUnderstompGrindChance = 20.0f;

    //Kick Settings
    bool bEnablePlayerPushBack = true;

    //Grab Settings
    bool bGrabStartIsHostile = true;

    //Hug Settings
    bool bNonLethalHugsHostile = true;
    bool bHugsStopAtFullHP = true;

    // Cleavage offset for forward/back and up/down adjustments
    std::array<float, 2> f2CleavageOffset = { 0.0f, 0.0f };
    
    //Cleavage Settings
    float fGrabPlayVoreOffset_Z = 0.0f;
    bool bShrinkBreastsOverTime = true;
    float fBreastShrinkRate = 0.15f;
    float fBreastsAbsorbIncrementBy = 1.85;
    bool bEnlargeBreastsOnAbsorption = true;



	//Morph Lists
    GameplayMorphSettings_t MorphListBreasts = {
        .fMultiplier = 1.0f,
		.MorphNames = {
		    "Breasts",
		    "BreastsFantasy",
		    "BreastsNewSH",
		    "BreastGravity2"
		},
		.MorphScales = {
		    0.45f,
		    0.20f,
		    0.30f,
		    0.15f
		}
    };

    GameplayMorphSettings_t MorphListBelly = {
        .fMultiplier = 1.0f,
        .MorphNames = {
            "PregnancyBelly",
        },
        .MorphScales = {
            0.20f,
        }
    };
};
TOML_SERIALIZABLE(GameplayActionSettings_t);

struct GameplayActorSettings_t {

    // Basic settings
    std::string sGameMode = "kNone";
    float fRandomGrowthDelay = 1.0f;

    // Growth/Shrink parameters
    float fGrowthRate = 0.001f;
    float fShrinkRate = 0.001f;
    float fCurseGrowthSizeLimit = 25.0f;

    // Curse-related settings
    float fCurseTargetScale = 1.5f;

    // Update settings
    float fGameModeUpdateInterval = 3.0f;
    bool bMultiplyGrowthrate = false;

    float fScalePerLevel = 0.05f;
	bool bUseGTSSkill = false;

};
TOML_SERIALIZABLE(GameplayActorSettings_t);

//-------------------------------------------------------------------------------------------------------------------
//  BASE STRUCT
//  (Directly Serialized)
//-------------------------------------------------------------------------------------------------------------------

struct SettingsGameplay_t {

    // Gamemode settings for different actor types
    GameplayActorSettings_t GamemodePlayer = {};
    GameplayActorSettings_t GamemodeFollower = {};
    GameplayActionSettings_t ActionSettings = {};

    // Size Effects
    bool bPlayerAnimEffects = true;
    bool bNPCAnimEffects = true;
    bool bLaunchObjects = true;
    bool bLaunchAllCells = false;

    // Cloth Tearing parameters
    bool bClothTearing = false;
    float fClothRipStart = 1.5f;
    float fClothRipThreshold = 2.2f;

    // Perk settings
    bool bEnableCrushGrowth = true;
    bool bEnableGrowthOnHit = true;
    float fFullAssimilationLevelCap = 1.0f;
    float fHitGrowthPower = 1.0f;

};
TOML_SERIALIZABLE(SettingsGameplay_t);
TOML_REGISTER_NAME(SettingsGameplay_t, "Gameplay");