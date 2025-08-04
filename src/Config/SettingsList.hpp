#pragma once

/* 

C++ lacks proper reflection support.
Because of this, doing any form of reflection is an absolute pain in the ass.

Thanks to this little library however we can somewhat achieve this.
https://github.com/qlibs/reflect

TOML11 versions > 4.0.0 provide some examples, using this library to deduce runtime information of user defined structs
which normally gets omitted during compilation thus allowing us to serialize / deserialize them into toml tables.

Since all of this is just one big hack, there are some limitations present namely:
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

enum class CameraTrackingSettings : uint8_t {
    kNone,
    kSpine,
    kClavicle,
    kBreasts,
    kBreasts_00,
    kBreasts_01,
    kBreasts_02,
    kBreasts_03,
    kBreasts_04,
    kNeck,
    kButt,
	kGenitals,
    kBelly,
};

enum class CameraModeTP : uint8_t {
    kNormal,
    kAlternative,
    kFootLeft,
    kFootRight,
    kFeetCenter
};

enum class DisplayUnit : uint8_t {
    kMetric,
    kImperial,
    kMammoth
};

enum class SizeMode : uint8_t {
    kNormal,
    kMassBased
};

enum class SelectedGameMode : uint8_t {
    kNone,
    kGrow,
    kShrink,
    kCombatGrowth,
    kSlowCombatGrowth,
    kCurseOfGrowth,
    kCurseOfTheGiantess,        //<------- TODO Implement these... AKA: Just take them from my dll
    kCurseOfDiminishing,
    kSizeLocked //<-------------- Combines both
};

//-------------------------------------------------------------------------------------------------------------------
//  CHILD STRUCTS 
//  (Not Directly Serialized, but used within other structs)
//-------------------------------------------------------------------------------------------------------------------


//------------------------------------------- Gameplay
struct GameplayActorSettings {
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
};
TOML_SERIALIZABLE(GameplayActorSettings);

struct GameplayActionSettings {

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
    float fGrabPlayVoreOffset_Z = 0.0f;

};
TOML_SERIALIZABLE(GameplayActionSettings);

//------------------------------------------- Camera
struct CameraOffsets {
    // Bone to center on
    std::string sCenterOnBone = "kNone";

    // Stand offsets
    std::array<float, 3> f3NormalStand = {0.0f, 0.0f, 0.0f};  
    std::array<float, 3> f3CombatStand = {0.0f, 0.0f, 0.0f};

    // Sneak/Crawl offsets
    std::array<float, 3> f3NormalCrawl = {0.0f, 0.0f, 0.0f}; 
    std::array<float, 3> f3CombatCrawl = {0.0f, 0.0f, 0.0f};
};
TOML_SERIALIZABLE(CameraOffsets);

//------------------------------------------- AI
// Stateless actions: simple probability-based triggers.
struct AIStatelessAction {
    bool bEnableAction = true;
    float fProbability = 50.0f;
};
TOML_SERIALIZABLE(AIStatelessAction);

// Statefull actions: have additional probability settings and an interval.
struct AIStatefullAction {
    bool bEnableAction = true;
    float fProbability = 50.0f;
    float fProbabilityLight = 50.0f;
    float fProbabilityHeavy = 50.0f;
    float fInterval = 2.0f;
};
TOML_SERIALIZABLE(AIStatefullAction);

// Complex action: Hug
struct AIHugAction {
    // Basic toggle and overall probability
    bool bEnableAction = true;
    float fProbability = 35.0f;

    // Conditions
    bool bStopIfCantShrink = true;
    bool bKillFriendlies = true;
    bool bKillFollowersOrPlayer = false;
    
    // Outcome probabilities
    float fHealProb = 30.0f;
    float fShrinkProb = 100.0f;
    float fFriendlyShrinkProb = 15.0f;
    float fKillProb = 30.0f;

    // Timing
    float fInterval = 1.5f;
};
TOML_SERIALIZABLE(AIHugAction);

struct AIStompAction {
    bool bEnableAction = true;
    float fProbability = 33.0f;
    float fUnderstompGrindProbability = 40.0f;
};
TOML_SERIALIZABLE(AIStompAction);

// Complex action: Butt Crush
struct AIButtAction {
    bool bEnableAction = true;
    float fProbability = 50.0f;

    // Different probabilities for variations of the action
    float fFastProb = 50.0f;
    float fButtCrushTypeProb = 50.0f;
    float fGrowProb = 35.0f;
    float fCrushProb = 35.0f;

    // Timing and limit
    float fInterval = 2.0f;
};
TOML_SERIALIZABLE(AIButtAction);

// Complex action: Grab
struct AIGrabAction {
    bool bEnableAction = true;
    float fProbability = 33.0f;

    // Different attack types
    float fThrowProb = 33.0f;
    float fVoreProb = 50.0f;
    float fCrushProb = 50.0f;
    float fReleaseProb = 0.0f;

    // Cleavage (multiple stages) probabilities
    float fCleavageProb = 65.0f;
    float fCleavageAttackProb = 35.0f;
    float fCleavageAbsorbProb = 35.0f;
    float fCleavageVoreProb = 35.0f;
    float fCleavageSuffocateProb = 35.0f;
    float fCleavageStopProb = 0.0f;
    float fStrangleChance = 35.0f;

    // Timing
    float fInterval = 1.5f;

};
TOML_SERIALIZABLE(AIGrabAction);

//------------------------------------------- Interface/GtsUI
struct WindowConfWidget {
    // Window lock and visibility
    bool bLock = true;
    bool bVisible = false;

    // Positioning and appearance
    std::array<float, 2> f2Offset = {20.0f, 20.0f};
    std::array<float, 3> f3ColorA = { 1.0f, 1.0f, 1.0f };
    std::array<float, 3> f3ColorB = { 1.0f, 1.0f, 1.0f };
    bool bEnableRounding = true;
    bool bUseCustomGradientColors = false;
    bool bUseGradient = true;
    bool bFlipGradientDirection = false;
    float fNormalGradientLightMult = 1.5f;
    float fNormalGradientDarkMult = 1.0f;
    float fBorderThickness = 2.0f;
    float fBorderLightness = 0.15f;
    float fBorderAlpha = 0.50f;


    std::string sAnchor = "kTopRight";
    float fAlpha = 1.0f;
    float fBGAlphaMult = 1.0f;

    bool bEnableFade = true;
    float fFadeAfter = 1.0f;
    float fFixedWidth = 100.0f;
    float fSizeBarHeightMult = 1.0f;
    float fFadeDelta = 0.0f;
    // Display control
    uint32_t iFlags = 0;
};
TOML_SERIALIZABLE(WindowConfWidget);

struct WindowConfSettings {
    bool bLock = true;
    std::array<float, 2> f2Offset = {0.0f, 0.0f};
    std::string sAnchor = "kCenter";
    float fWindowSize = 90.f;
    float fAlpha = 1.0f;
    float fBGAlphaMult = 0.45f;
};
TOML_SERIALIZABLE(WindowConfSettings);

//-------------------------------------------------------------------------------------------------------------------
//  PARENT STRUCTS 
//  (Serialized directly, with instances assumed in the config singleton)
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
//  HIDDEN
//-------------------------------------------------------------------------------------------------------------------
// This struct is only loaded (not saved) and enables the advanced category in the configuration window.
struct SettingsHidden {
    bool IKnowWhatImDoing = false;
};
TOML_SERIALIZABLE(SettingsHidden);

//-------------------------------------------------------------------------------------------------------------------
//  DEBUG / ADVANCED
//-------------------------------------------------------------------------------------------------------------------
struct SettingsAdvanced {
    // Logging levels
    std::string sLogLevel = "err";

    // Toggles for advanced features
    bool bShowOverlay = false;
    bool bDamageAV = true;
    bool bCooldowns = true;
    bool bEnforceUIClamps = true;
    bool bPauseGame = true;
    bool bEnlargeBreastsOnAbsorption = false;
    bool bPlayerAI = false;
    float fSGTMMult = 0.5f;
    float fAnimSpeedAdjMultPlayer = 1.0f;
    float fAnimSpeedAdjMultTeammate = 1.0f;
    bool bEnableExperimentalDevourmentAI = false;
    float fExperimentalDevourmentAIProb = 25.0f;
    std::array<float, 5> fAnimSpeedFormula = { 0.142f, 0.82f, 1.90f, 1.0f, 0.0f };

};
TOML_SERIALIZABLE(SettingsAdvanced);


//-------------------------------------------------------------------------------------------------------------------
//  AUDIO
//-------------------------------------------------------------------------------------------------------------------
struct SettingsAudio {
    // Footstep sounds
    bool bFootstepSounds = true;
    bool bUseOldSounds = false;

    // Voice settings
    bool bSlowGrowMoans = true;
    bool bEnableVoiceOverride = true;
    bool bMuteVoreDeathScreams = true;
    bool bMuteHugCrushDeathScreams = true;
    bool bMuteFingerSnapDeathScreams = true;
    bool bMuteBreastAbsorptionDeathScreams = true;
    bool bMuteShrinkToNothingDeathScreams = false;
    bool bMuteCrushDeathScreams = false;
    

    bool bMoanLaughSizeVariants = true;
    bool bMoanLaughPCExclusive = false;
    bool bUseOtherHighHeelSet = true;
    bool bBlendBetweenFootsteps = false;
    
    float fMaxVoiceFrequency = 1.0f;

    float fFallOffMultiplier = 1.0f;
    float fVoiceVolumeMult = 1.0f;

};
TOML_SERIALIZABLE(SettingsAudio);


//-------------------------------------------------------------------------------------------------------------------
//  GENERAL
//-------------------------------------------------------------------------------------------------------------------
struct SettingsGeneral {
    // Visual settings
    bool bLessGore = false;
    bool bShowHearts = true;
    bool bShowIcons = true;

    // Compatibility options
    bool bDevourmentCompat = false;
    bool bConversationCamCompat = false;
    bool bOverrideInteractionDist = false;
    bool bAlternativeSpeedFormula = false;

    // Protection toggles
    bool bProtectEssentials = true;
    bool bProtectFollowers = true;

    // Gameplay enhancements
    bool bDynamicSizePlayer = false;
    bool bDynamicSizeFollowers = false;
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
};
TOML_SERIALIZABLE(SettingsGeneral);

//-------------------------------------------------------------------------------------------------------------------
//  GAMEPLAY
//-------------------------------------------------------------------------------------------------------------------
struct SettingsGameplay {
    // Gamemode settings for different actor types
    GameplayActorSettings GamemodePlayer = {};
    GameplayActorSettings GamemodeFollower = {};
    GameplayActionSettings ActionSettings = {};

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
    float fSizeConvLevelCap = 1.0f;

};
TOML_SERIALIZABLE(SettingsGameplay);

//-------------------------------------------------------------------------------------------------------------------
//  CAMERA
//-------------------------------------------------------------------------------------------------------------------
struct SettingsCamera {
    // Shake and crawl multipliers
    float fCameraShakePlayer = 1.0f;
    float fCameraShakePlayerFP = 0.2f;
    float fCameraShakeOther = 1.0f;
    float fFPCrawlHeightMult = 0.40f;
    float fTPCrawlHeightMult = 0.40f;
    
    // Offset settings (for first-person and third-person)
    CameraOffsets OffsetsNormal = {};
    CameraOffsets OffsetsAlt = {};
    
    // Automatic camera controls
    bool bAutomaticCamera = true;
    float fCameraInterpolationFactor = 0.35f;

    // Distance and zoom controls

    bool bEnableAutoFNearDist = false;

    bool bEnableSkyrimCameraAdjustments = true;

    float fCameraDistMin = 150.0f;
    float fCameraDistMax = 600.0f;
    float fCameraZoomSpeed = 1.2f;
    float fCameraIncrement = 0.075f;

    // Collision settings
    bool bCamCollideActor = false;
    bool bCamCollideTree = false;
    bool bCamCollideDebris = true;
    bool bCamCollideTerrain = true;
    bool bCamCollideStatics = true;
    float fModifyCamCollideAt = 3.0f;
};
TOML_SERIALIZABLE(SettingsCamera);


//-------------------------------------------------------------------------------------------------------------------
//  BALANCE
//-------------------------------------------------------------------------------------------------------------------
struct SettingsBalance {
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
TOML_SERIALIZABLE(SettingsBalance);


//-------------------------------------------------------------------------------------------------------------------
//  AI
//-------------------------------------------------------------------------------------------------------------------
struct SettingsAI {
    // Global AI settings
    bool bEnableActionAI = true;
    float fMasterTimer = 3.0f;

    // Stateless Actions
    AIStatelessAction Vore = { .bEnableAction = true, .fProbability = 33.0f };
    AIStatelessAction KickSwipe = { .bEnableAction = true, .fProbability = 25.0f };

    // Statefull Actions
    AIStatefullAction ThighCrush = { .bEnableAction = true, .fProbability = 15.0f, .fInterval = 1.0f };
    AIStatefullAction ThighSandwich = { .bEnableAction = true, .fProbability = 20.0f, .fInterval = 1.5f };

    // Complex Actions
    AIStompAction Stomp = {};
    AIHugAction Hugs = {};
    AIButtAction ButtCrush = {};
    AIGrabAction Grab = {};

    // Additional AI toggles
    bool bPanic = true;
    bool bCombatOnly = true;
    bool bAllowPlayer = true;
    bool bAllowFollowers = true;
    bool bHostileOnly = true;
    bool bDisableAttacks = true;
    bool bAlwaysDisableAttacks = false;
    bool bSlowMovementDown = true;
    bool bSlowRotationDown = true;
    bool bRecordBoneSpeedData = false;
};
TOML_SERIALIZABLE(SettingsAI);


//-------------------------------------------------------------------------------------------------------------------
//  GtsUI
//-------------------------------------------------------------------------------------------------------------------
struct SettingsUI {
    // Display settings
    std::string sDisplayUnits = "kMetric";
    float fScale = 1.0f;
    float fItemWidth = 0.55f;
    std::array<float, 3> f3AccentColor = { 0.81834f, 0.797923f, 0.834302f }; // Default Menu UI Color

    // Red: 0.273f, 0.0106f, 0.0106f
    // White: 0.81834f, 0.797923f, 0.834302f
    //std::array<float, 3> f3AccentColor = {0.486f, 0.431f, 0.529f};
    std::array<float, 3> StatusAccentColor =        {  0.273f, 0.0106f, 0.0106f }; // Default Status Bar Color
    std::array<float, 3> UnderStompAccentColor =    {  0.273f, 0.0106f, 0.0106f }; // Default Understomp Bar Color

    // Window configurations
    WindowConfSettings SettingsWindow {};


    WindowConfWidget StatusWindow = {

        .bLock = true,
        .bVisible = true,
    	.f2Offset = {0.0f, 28.5f},
    	.f3ColorA = StatusAccentColor,
        .f3ColorB = StatusAccentColor,
    	.sAnchor = "kCenter",
        .fAlpha = 1.0f,
        .fBGAlphaMult = 0.0f,
        .bEnableFade = true,
        .fFadeAfter = 1.8f,
        .fFixedWidth = 150.0f,
        .fSizeBarHeightMult = 0.1f,
        .fFadeDelta = 0.01f,
        .iFlags = 3,

    };

    WindowConfWidget UnderstompWindow = {

        .bLock = true,
        .bVisible = true,
        .f2Offset = {0.0f, 60.0f},
    	.f3ColorA = UnderStompAccentColor,
        .f3ColorB = UnderStompAccentColor,
        .sAnchor = "kCenter",
        .fAlpha = 1.0f,
        .fBGAlphaMult = 0.0f,
        .bEnableFade = true,
        .fFadeAfter = 1.8f,
        .fFixedWidth = 120.0f,
        .fSizeBarHeightMult = 0.01f,
        .fFadeDelta = 0.05f,
        .iFlags = 1,

    };

};
TOML_SERIALIZABLE(SettingsUI);