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

enum class LCameraTrackBone_t : uint8_t {
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

enum class LCameraMode_t : uint8_t {
    kNormal,
    kAlternative,
    kFootLeft,
    kFootRight,
    kFeetCenter
};

//-------------------------------------------------------------------------------------------------------------------
//  CHILD STRUCTS 
//  (Not Directly Serialized, but used within other structs)
//-------------------------------------------------------------------------------------------------------------------

struct CameraOffsets_t {

    // Bone to center on
    std::string sCenterOnBone = "kNone";

    // Stand offsets
    std::array<float, 3> f3NormalStand = { 0.0f, 0.0f, 0.0f };
    std::array<float, 3> f3CombatStand = { 0.0f, 0.0f, 0.0f };

    // Sneak/Crawl offsets
    std::array<float, 3> f3NormalCrawl = { 0.0f, 0.0f, 0.0f };
    std::array<float, 3> f3CombatCrawl = { 0.0f, 0.0f, 0.0f };

};
TOML_SERIALIZABLE(CameraOffsets_t);


//-------------------------------------------------------------------------------------------------------------------
//  BASE STRUCTS
//  (Directly Serialized)
//-------------------------------------------------------------------------------------------------------------------

struct SettingsCamera_t {

    // Shake and crawl multipliers
    float fCameraShakePlayer = 1.0f;
    float fCameraShakePlayerFP = 0.2f;
    float fCameraShakeOther = 1.0f;
    float fFPCrawlHeightMult = 0.40f;
    float fTPCrawlHeightMult = 0.40f;

    // Offset settings (for first-person and third-person)
    CameraOffsets_t OffsetsNormal = {};
    CameraOffsets_t OffsetsAlt = {};

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
TOML_SERIALIZABLE(SettingsCamera_t);
TOML_REGISTER_NAME(SettingsCamera_t, "Camera");