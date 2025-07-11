#pragma once

namespace GTS {
	// Module that handles multiplication of pushing others away
    enum class LaunchType {
        Actor_Launch,
        Object_Towards,
        Object_Launch,
    };

	float GetLaunchPowerFor(Actor* giant, float SizeDifference, LaunchType Type);
    float GetForceFromDistance(float distance, float maxDistance);
}