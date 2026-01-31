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
        Action_AbsorbOther,
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
        Misc_TinyCalamityRage,
        Footstep_Right,
        Footstep_Left,
        Footstep_JumpLand,
        Emotion_Voice,
        Emotion_Voice_Long,
    };

    struct CooldownData {
        double lastPushTime = -1.0e8f;
        double lastHandDamageTime = -1.0e8f;
        double lastLaunchTime = -1.0e8f;
        double lastHealthGateTime = -1.0e8f;
        double lastThighDamageTime = -1.0e8f;
        double lastButtCrushTime = -1.0e8f;
        double lastScareTime = -1.0e8f;
        double lastHugTime = -1.0e8f;

        double lastAbsorbTime = -1.0e8f;

        double lastBreastAbsorbTime = -1.0e8f;
        double lastBreastSuffocateTime = -1.0e8f;
        double lastBreastVoreTime = -1.0e8f;
        
        double lastLaughTime = -1.0e8f;
        double lastMoanTime = -1.0e8f;
        double lastMoanCrushTime = -1.0e8f;

        double lastRevertTime = -1.0e8f;
        double lastSoundGrowthTime = -1.0e8f;
        double lastHitTime = -1.0e8f;
        double lastGrowthTime = -1.0e8f;
        double lastOutburstTime = -1.0e8f;
        double lastForceOutburstTime = -1.0e8f;
        double lastShrinkParticleTime = -1.0e8f;
        double lastAnimShrinkParticleTime = -1.0e8f;
        double lastGazeShrinkParticleTime = -1.0e8f;
        double lastTinyCalamityTime = -1.0e8f;

        double lastFootstepTime_R = -1.0e8f;
        double lastFootstepTime_L = -1.0e8f;
        double lastJumplandTime = -1.0e8f;

        double lastEmotionTime = -1.0e8f;
        double lastEmotionTime_Long = -1.0e8f;
    };

    void ApplyActionCooldown(Actor* giant, CooldownSource source);
    double GetRemainingCooldown(Actor* giant, CooldownSource source);
    bool IsActionOnCooldown(Actor* giant, CooldownSource source);

    class CooldownManager : public GTS::EventListener, public CInitSingleton <CooldownManager> {
		public:
		virtual std::string DebugName() override;
		virtual void Reset() override;
		CooldownData& GetCooldownData(Actor* actor);

        private: 
        std::unordered_map<Actor*, CooldownData> CooldownData;
    };
}