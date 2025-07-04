#pragma once

namespace GTS {

    const enum class GTSInfoFeatures : uint32_t {
        kUnitScale = (1 << 0),
        kUnitReal  = (1 << 1),
        kMaxSize = (1 << 2),
        kAspect = (1 << 3),
        kWeight = (1 << 4),
        kDamageResist = (1 << 5),
        kCarryWeight = (1 << 6),
        kSpeedMultiplier = (1 << 7),
        kJumpMultiplier = (1 << 8),
        kDamageMultiplier = (1 << 9),
        kAutoSize = (1 << 10),
        kBonusSize = (1 << 11),
        kShrinkResist = (1 << 12),
        kOnTheEdge = (1 << 13),
        kStolenAttributes = (1 << 14),
        kAbsorbedAttributes = (1 << 15),
        kStolenStamina = (1 << 16),
        kStolenMagicka = (1 << 17),
        kSizeReserve = (1 << 18),
        kHeelsBonusDamage = (1 << 19),
        kOverkillCounter = (1 << 20),
    };


    // Enable bitwise operations for the enum
    constexpr GTSInfoFeatures operator|(GTSInfoFeatures lhs, GTSInfoFeatures rhs) {
        return static_cast<GTSInfoFeatures>(
            static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)
        );
    }

    constexpr GTSInfoFeatures operator&(GTSInfoFeatures lhs, GTSInfoFeatures rhs) {
        return static_cast<GTSInfoFeatures>(
            static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)
        );
    }

    constexpr bool hasFlag(GTSInfoFeatures value, GTSInfoFeatures flag) {
        return (value & flag) == flag;
    }

    void DrawGTSSizeBar(GTSInfoFeatures a_featureFlags, RE::Actor* a_Actor, const bool a_IsWidget);
    inline void DrawGTSSizeBar(uint32_t a_featureFlags, RE::Actor* a_Actor, const bool a_IsWidget) {
        DrawGTSSizeBar(static_cast<GTSInfoFeatures>(a_featureFlags), a_Actor, a_IsWidget);
    }


    void DrawGTSInfo(GTSInfoFeatures a_featureFlags, RE::Actor* a_Actor, const bool a_IsWidget);
    inline void DrawGTSInfo(uint32_t a_featureFlags, RE::Actor* a_Actor, const bool a_IsWidget) {
        DrawGTSInfo(static_cast<GTSInfoFeatures>(a_featureFlags), a_Actor, a_IsWidget);
    }
}

template <>
struct magic_enum::customize::enum_range<GTS::GTSInfoFeatures> {
    static constexpr bool is_flags = true;
};
