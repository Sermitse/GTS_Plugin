
#include "Managers/Damage/LaunchPower.hpp"

#include "Config/Config.hpp"

#include "Managers/HighHeel.hpp"
#include "Debug/DebugDraw.hpp"

using namespace GTS;

namespace {
    std::string to_string(LaunchType type) {
        switch (type) {
            case LaunchType::Actor_Towards: return "Actor_Towards";
            case LaunchType::Actor_Launch: return "Actor_Launch";
            case LaunchType::Object_Towards: return "Object_Towards";
            case LaunchType::Object_Launch: return "Object_Launch";
            default: return "Unknown";
        }
    }
}

namespace GTS {
    float GetLaunchPowerFor(Actor* giant, float SizeDifference, LaunchType Type, float incoming_Formula) {
        float multiplier = 3.0f, limit = 20.0f;

        switch (Type) {
            case LaunchType::Actor_Towards:     multiplier = 6.00f;     limit = 1500.0f;    break;
            case LaunchType::Actor_Launch:      multiplier = 6.00f;     limit = 1250.0f;    break;
            case LaunchType::Object_Towards:    multiplier = 6.00f;     limit = 15.0f;      break;
            case LaunchType::Object_Launch:     multiplier = 3.00f;     limit = 10.0f;      break;
            default:                            multiplier = 3.00f;     limit = 20.0f;      break;
        }

        
        float power = std::cbrt(SizeDifference) * multiplier * incoming_Formula;
        float result = std::min(power, limit);

        if (Type == LaunchType::Actor_Launch) {
            log::info("Incoming Formula: {}", incoming_Formula);
            log::info("Power: {}", power);
            log::info("Result: {}", result);
        }

        //log::info("Push Result: {}, {}", result, to_string(Type));
        return result;
    }

    float GetForceFromDistance(float distance, float maxDistance) { // Obtains Force based on the distance between two objects (foot/enemy for example)
        // This should always be called when Distance <= MaxDistance to avoid values > than 1.0f
        // If Distance = 365 and MaxDistance = 320 then it will be > than 1.0, don't do that.
        float safeMax = std::max(maxDistance, 0.001f);
        float ratio = std::clamp(distance / safeMax, 0.0f, 1.0f);
        return 1.0f - ratio;
    }
}