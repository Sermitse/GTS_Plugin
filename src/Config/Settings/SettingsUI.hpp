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
//  CHILD STRUCTS 
//  (Not Directly Serialized, but used within other structs)
//-------------------------------------------------------------------------------------------------------------------

struct WindowConfWidget_t {
    // Window lock and visibility
    bool bLock = true;
    bool bVisible = false;

    // Positioning and appearance
    std::array<float, 2> f2Offset = { 20.0f, 20.0f };
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
TOML_SERIALIZABLE(WindowConfWidget_t);

struct WindowConfSettings_t {
    bool bLock = true;
    std::array<float, 2> f2Offset = { 0.0f, 0.0f };
    std::string sAnchor = "kCenter";
    float fWindowSize = 90.f;
    float fAlpha = 1.0f;
    float fBGAlphaMult = 0.45f;
};
TOML_SERIALIZABLE(WindowConfSettings_t);

//-------------------------------------------------------------------------------------------------------------------
//  BASE STRUCT
//  (Directly Serialized)
//-------------------------------------------------------------------------------------------------------------------

struct SettingsUI_t {

    // Display settings
    std::string sDisplayUnits = "kMetric";
    float fScale = 1.0f;
    float fItemWidth = 0.55f;

    std::array<float, 3> f3AccentColor = { 0.81834f, 0.797923f, 0.834302f }; // Default Menu UI Color

    // Red: 0.273f, 0.0106f, 0.0106f
    // White: 0.81834f, 0.797923f, 0.834302f
    //std::array<float, 3> f3AccentColor = {0.486f, 0.431f, 0.529f};
    std::array<float, 3> StatusAccentColor = { 0.273f, 0.0106f, 0.0106f }; // Default Status Bar Color
    std::array<float, 3> UnderStompAccentColor = { 0.273f, 0.0106f, 0.0106f }; // Default Understomp Bar Color
    // Window configurations

    WindowConfSettings_t SettingsWindow{};

    WindowConfWidget_t StatusWindow = {
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

    WindowConfWidget_t UnderstompWindow = {
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
TOML_SERIALIZABLE(SettingsUI_t);
TOML_REGISTER_NAME(SettingsUI_t, "UI");


// Base window settings that all windows inherit
struct BaseWindowSettings_t {
    float fWindowSize = 90.f;
    bool bLock = false;
    bool bVisible = true;
    std::array<float, 2> f2Offset = { 0.0f, 0.0f };
    std::array<float, 3> f3ColorA = { 1.0f, 1.0f, 1.0f };
    std::array<float, 3> f3ColorB = { 1.0f, 1.0f, 1.0f };
    std::string sAnchor = "kCenter";
    float fAlpha = 1.0f;
    float fBGAlphaMult = 0.5f;
};
TOML_SERIALIZABLE(BaseWindowSettings_t);
TOML_REGISTER_NAME(BaseWindowSettings_t, "UI2");






















