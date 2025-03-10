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
        kEssence = (1 << 11),
        kBonusSize = (1 << 12),
        kShrinkResist = (1 << 13),
        kOnTheEdge = (1 << 14),
        kStolenAttributes = (1 << 15),
        kStolenHealth = (1 << 16),
        kStolenStamina = (1 << 17),
        kStolenMagicka = (1 << 18),
        kSizeReserve = (1 << 19),
        kHeelsBonusDamage = (1 << 20),
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

    void DrawGTSInfo(const GTSInfoFeatures a_featureFlags, RE::Actor* a_Actor);
}

template <>
struct magic_enum::customize::enum_range<GTS::GTSInfoFeatures> {
    static constexpr bool is_flags = true;
};
