#pragma once

namespace GTS {
    

    // Result of probing one aim "zone" (close or farZone), without side effects.
    template <typename Fn>
    AimAttempt TryAim(Fn&& fn) {
        AimAttempt a;
        a.hit = fn(a.left, a.result);
        return a;
    }

    void RandomizeBlend(Actor* giant, bool left);
    bool AutoAim_And_DetermineStompType(Actor* giant, bool& left, bool strong_Attack = false, bool trample = false);
    bool CrosshairUnderstomp(Actor* giant);

    static inline float Remap(float x, float in_min, float in_max, float out_min, float out_max) {
        return out_min + (x - in_min) * (out_max - out_min) / (in_max - in_min);
    }
}