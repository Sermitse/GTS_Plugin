#pragma once


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
        Misc_AutoAim_Cooldown,
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

        double& GetLastTime(Actor* actor, CooldownSource source);

        private:
        std::unordered_map<Actor*, std::array<double, static_cast<size_t>(CooldownSource::Total)>> _lastActionTimes;
        Actor* _lastAccessedActor = nullptr;
        std::array<double, static_cast<size_t>(CooldownSource::Total)>* _lastAccessedTimes = nullptr;
    };
}