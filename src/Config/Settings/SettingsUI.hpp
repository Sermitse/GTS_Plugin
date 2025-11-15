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
* L(x)
* stucts don't get a special notation
*/

//-------------------------------------------------------------------------------------------------------------------
//  ENUMS ----- Assumed to be the reference values 
//  magic_enum will use to convert an enum to a string representation for serialization (Saving The TOML)
//-------------------------------------------------------------------------------------------------------------------

enum class LDisplayUnit_t : uint8_t {
    kMetric,
    kImperial,
    kMammoth
};

//-------------------------------------------------------------------------------------------------------------------
//  BASE STRUCT
//  (Directly Serialized)
//-------------------------------------------------------------------------------------------------------------------

struct SettingsUI_t {

    // Display settings
    std::string sDisplayUnits = "kMetric";
    float fScale = 1.0f;
    float fItemWidth = 0.55f;
    float fSGTMMult = 0.7f;
    bool bDoBGBlur = true;
    bool bDoPause = true;
    bool bDoSlowdown = true;

    // Red: 0.273f, 0.0106f, 0.0106f
	// White: 0.81834f, 0.797923f, 0.834302f
    std::array<float, 3> f3AccentColor = { 0.273f, 0.0106f, 0.0106f }; // Default Menu UI Color
    std::array<float, 3> f3IconOverflowColor = { 0.273f, 0.0106f, 0.0106f };


};
TOML_SERIALIZABLE(SettingsUI_t);
TOML_REGISTER_NAME(SettingsUI_t, "UI");

//-------------------------------------------------------------------------------------------------------------------
//  Dynamics
//  (Used by ImConfigurableWindow)
//  Note: Most defaults are set in the Window Init for that particular window instead of here
//-------------------------------------------------------------------------------------------------------------------

// Base window settings that all windows inherit
// This Type is always known
struct WindowSettingsBase_t {
    bool bLock = true;
    std::array<float, 2> f2Position = { 0.0f, 0.0f };
    std::string sAnchor = "kCenter";
    float fAlpha = 1.0f;
    float fBGAlphaMult = 1.0f;
    float fWindowSizePercent = 90.0f;

    bool bVisible = true;
    bool bEnableFade = true;
    float fFadeAfter = 2.5f;
    float fFadeDelta = 0.01f;

};
TOML_SERIALIZABLE(WindowSettingsBase_t);
TOML_REGISTER_NAME(WindowSettingsBase_t, "Window");

//Extended settings for the sizebars
struct WindowSettingsSizeBar_t {
    bool bShowName = false;
    bool bShowScale = true;
    bool bShowSize = true;
    float fBorderThickness = 1.0f;
    float fBorderLightness = 1.0f;
    float fRounding = 5.0f;
    float fBorderAlpha = 1.0f;
    std::array<float, 2> f2GradientRange = { 1.0f, 1.0f };
    std::array<float, 3> f3ColorA = { 1.0f, 1.0f, 1.0f };
    std::array<float, 3> f3ColorB = { 1.0f, 1.0f, 1.0f };
	std::array<float, 2> f2Size = { 150.0f, 1.0f };
    uint16_t iFlags = 0;
};
TOML_SERIALIZABLE(WindowSettingsSizeBar_t);
TOML_REGISTER_NAME(WindowSettingsSizeBar_t, "SizeBar");

//Extended settings for the understomp bar, mostly a copy of the above one.
struct WindowSettingsUnderstompBar_t {
    bool bShowScale = true;
    bool bShowAbsoluteAngle = false;
    float fBorderThickness = 1.0f;
    float fBorderLightness = 1.0f;
    float fRounding = 5.0f;
    float fBorderAlpha = 1.0f;
    std::array<float, 2> f2GradientRange;
    std::array<float, 3> f3ColorA;
    std::array<float, 3> f3ColorB;
    std::array<float, 2> f2Size;
    uint16_t iFlags = 0;
};
TOML_SERIALIZABLE(WindowSettingsUnderstompBar_t);
TOML_REGISTER_NAME(WindowSettingsUnderstompBar_t, "USBar");

//Extended settings for the status/buff/icons bar
struct WindowSettingsStatusBar_t {
    uint16_t iIconSize = 48;
    uint16_t iFlagsVis = 0;
    uint16_t iFlagsAS = 0;
    std::array<float, 3> f3BGColor = { 0.0f, 0.0f, 0.0f };
};
TOML_SERIALIZABLE(WindowSettingsStatusBar_t);
TOML_REGISTER_NAME(WindowSettingsStatusBar_t, "StatusBar");


struct WindowSettingsKillFeed_t {
    uint16_t iFlags = 0;
    float fWidth = 400.0f;
    uint8_t iMaxVisibleEntries = 8;
    std::array<float, 3> f3BGColor = {0.0f, 0.0f, 0.0f};
};
TOML_SERIALIZABLE(WindowSettingsKillFeed_t);
TOML_REGISTER_NAME(WindowSettingsKillFeed_t, "KillFeed");