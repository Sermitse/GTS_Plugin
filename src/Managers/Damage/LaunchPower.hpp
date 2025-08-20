#pragma once

namespace GTS {
	// Module that handles multiplication of pushing others away
    enum class LaunchType {
        Actor_Towards,
        Actor_Launch,
        Object_Towards,
        Object_Launch,
    };

	float GetLaunchPowerFor(Actor* giant, float SizeDifference, LaunchType Type, float incoming_Formula = 1.0f);
    float GetForceFromDistance(float distance, float maxDistance);
}