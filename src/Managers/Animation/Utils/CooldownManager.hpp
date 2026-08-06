#pragma once
#include <array>

namespace GTS {

    enum class CooldownSource {
        Damage_Launch,
        Damage_Hand,
        Damage_Thigh,
        Push_Basic,
        Action_ButtCrush,
        Action_HealthGate,
        Action_ScareOther,
        Action_HugAbsorbOther,
        Action_Breasts_Absorb,
        Action_Breasts_Suffocate,
        Action_Breasts_Vore,
        Action_Hugs,
        Emotion_Laugh,
        Emotion_Moan,
        Emotion_Moan_Crush,
        Misc_RevertSound,
        Misc_GrowthSound,
        Misc_BeingHit,
        Misc_AiGrowth,
        Misc_ShrinkOutburst,
        Misc_ShrinkOutburst_Forced,
        Misc_ShrinkParticle,
        Misc_ShrinkParticle_Animation,
        Misc_ShrinkParticle_Gaze,
        Misc_TinyCalamity_WrathfulCalamity,
        Misc_TinyCalamity_Shrink,
        Misc_TinyCalamity_Hit,
        Misc_TinyCalamity_RunPushAway,
        Misc_TinyCalamity_Ragdoll,
        Footstep_Right,
        Footstep_Left,
        Footstep_JumpLand,
        Emotion_Voice,
        Emotion_Voice_Long,
        Total // keep this one last, sizes the arrays below
    };

    struct CooldownConfig {
        float staticCooldown = 0.0f;
        std::function<float(Actor*)> dynamicCooldown = nullptr; // nullptr -> use `staticCooldown` as-is

        float Get(Actor* actor) const {
            return dynamicCooldown ? dynamicCooldown(actor) : staticCooldown;
        }
    };

    const CooldownConfig& GetCooldownConfig(CooldownSource source);

    float Calculate_ShrinkOutburstTimer(Actor* actor);
    float Calculate_BreastActionCooldown(Actor* giant, int type);
    float Calculate_HugCrushCooldown(Actor* giant);
    float Calculate_ButtCrushTimer(Actor* actor);

    void ApplyActionCooldown(Actor* giant, CooldownSource source);
    float GetRemainingCooldown(Actor* giant, CooldownSource source);
    bool IsActionOnCooldown(Actor* giant, CooldownSource source);

    class CooldownManager : public GTS::EventListener, public CInitSingleton<CooldownManager> {
        public:
        virtual std::string DebugName() override;
        virtual void Reset() override;

        // Generic per-(actor, source) "last triggered" timestamp - replaces
        // the old CooldownData struct's one-named-field-per-source layout.
        // Adding a source never touches this.
        double& GetLastTime(Actor* actor, CooldownSource source);

        private:
        struct ActorCooldowns {
            Actor* actor = nullptr;
            std::array<double, static_cast<size_t>(CooldownSource::Total)> times;
        };

        // Flat vector + linear scan instead of a hash map: only a handful of
        // actors (3-5, occasionally more) are ever tracked at once, and at
        // that size a scan over contiguous memory beats hashing + chasing a
        // bucket/node pointer - no per-actor heap allocation, everything
        // fits in a cache line or two. _lastAccessedIndex additionally skips
        // the scan entirely for the common case of several cooldown checks
        // in a row on the same actor within one frame. An index (not a
        // pointer) is used so it stays valid across vector reallocation.
        std::vector<ActorCooldowns> _lastActionTimes;
        int _lastAccessedIndex = -1;
    };
}