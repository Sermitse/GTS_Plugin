#include "Managers/Animation/Utils/CooldownManager.hpp"

#include "Config/Config.hpp"

#include "Managers/Animation/AnimationManager.hpp"

using namespace GTS;

namespace {

	constexpr float LAUNCH_COOLDOWN = 1.8f;
	constexpr float PUSH_COOLDOWN = 2.0f;
	constexpr float HANDDAMAGE_COOLDOWN = 0.6f;
	constexpr float THIGHDAMAGE_COOLDOWN = 1.2f;

	constexpr float ABSORB_OTHER_COOLDOWN = 30.0f;

	constexpr float BREAST_SUFFOCATE_OTHER_COOLDOWN = 30.0f;
	constexpr float BREAST_ABSORB_OTHER_COOLDOWN = 30.0f;
	constexpr float BREAST_VORE_OTHER_COOLDOWN = 30.0f;

	constexpr float TINYCALAMITY_ONESHOT_COOLDOWN = 60.0f;

	constexpr float HEALTHGATE_COOLDOWN = 60.0f;
	constexpr float SCARE_COOLDOWN = 6.0f;
	constexpr float BUTTCRUSH_COOLDOWN = 30.0f;
	constexpr float HUGS_COOLDOWN = 10.0f;

	constexpr float LAUGH_COOLDOWN = 3.5f;
	constexpr float MOAN_COOLDOWN = 5.0f;
	constexpr float MOAN_CRUSH_COOLDOWN = 3.0f;

	constexpr float SOUND_COOLDOWN = 2.0f;
	constexpr float GROW_SOUND_COOLDOWN = 1.0f;

	constexpr float HIT_COOLDOWN = 1.0f;
	constexpr float AI_GROWTH_COOLDOWN = 2.0f;
	constexpr float SHRINK_OUTBURST_COOLDOWN = 18.0f;
	constexpr float SHRINK_OUTBURST_COOLDOWN_FORCED = 180.0f;
	constexpr float SHRINK_PARTICLE_COOLDOWN = 0.25f;
	constexpr float SHRINK_PARTICLE_COOLDOWN_GAZE = 0.25f;
	constexpr float SHRINK_PARTICLE_COOLDOWN_ANIM = 1.5f;
	constexpr float SHRINK_TINYCALAMITY_RAGE = 60.0f;

    constexpr float EMOTION_COOLDOWN = 1.5f;
    constexpr float EMOTION_COOLDOWN_LONG = 3.5f;
    

    float Calculate_BreastActionCooldown(Actor* giant, int type) {
        float Cooldown = 1.0;
        float mastery = 0.0;
        switch (type) {
            case 0:
                Cooldown = BREAST_SUFFOCATE_OTHER_COOLDOWN;
            break;
            case 1:
                Cooldown = BREAST_VORE_OTHER_COOLDOWN;
            break;
            case 2:
                Cooldown = BREAST_ABSORB_OTHER_COOLDOWN;
            break;
            default: 
                Cooldown = BREAST_SUFFOCATE_OTHER_COOLDOWN;
            break;   
        }

        if (Runtime::HasPerk(giant, "GTSPerkBreastsMastery1")) {
            float level = GetGtsSkillLevel(giant) - 40.0f; // Start past level 40
            mastery = std::clamp(level * 0.01f, 0.0f, 0.6f);
        }
        float reduction = 1.0f - mastery;

        return Cooldown * reduction;
    }

    float Calculate_AbsorbCooldown(Actor* giant) {
        float mastery = std::clamp(GetGtsSkillLevel(giant) * 0.01f, 0.0f, 1.0f) * 0.73f;
        float reduction = 1.0f - mastery; // Up to 8.1 seconds at level 100

        return ABSORB_OTHER_COOLDOWN * reduction;
    }

    float Calculate_ButtCrushTimer(Actor* actor) {
		bool lvl70 = Runtime::HasPerk(actor, "GTSPerkButtCrushAug3");
		bool lvl100 = Runtime::HasPerk(actor, "GTSPerkButtCrushAug4");
		float reduction = 1.0f;
		if (lvl100) { // 15% reduction
			reduction -= 0.15f;
		} if (lvl70) { // 10% reduction
			reduction -= 0.10f;
		} 
		return BUTTCRUSH_COOLDOWN * reduction;
	}

    float Calculate_FootstepTimer(Actor* actor) {
        float cooldown = 0.2f;
        cooldown /= AnimationManager::GetAnimSpeed(actor);
        //log::info("Cooldown for footstep: {}", cooldown);
        return cooldown;
    }

    float Calculate_ShrinkOutbirstTimer(Actor* actor) {
        bool DarkArts3 = Runtime::HasPerk(actor, "GTSPerkDarkArtsAug3");
        bool HealthRegen = Runtime::HasPerk(actor, "GTSPerkGrowthAug2");
        bool DarkArts_Legendary = Runtime::HasPerk(actor, "GTSPerkDarkArtsLegendary");
        float reduction = 1.0f;
        if (DarkArts3) {
            reduction = 0.7f;
        }
        if (HealthRegen && IsGrowthSpurtActive(actor)) {
            reduction *= 0.75f;
        }
        if (DarkArts_Legendary) {
            reduction *= 0.75f;
        }
        return SHRINK_OUTBURST_COOLDOWN * reduction;
    }

    float Calculate_EmotionCooldown(Actor* actor) {
        return EMOTION_COOLDOWN * AnimationManager::GetAnimSpeed(actor);
    }
    float Calculate_EmotionCooldown_Long(Actor* actor) {
        return EMOTION_COOLDOWN_LONG * AnimationManager::GetAnimSpeed(actor);
    }
    float Calculate_LaughCooldown(Actor* actor) {
        return LAUGH_COOLDOWN * AnimationManager::GetAnimSpeed(actor);
    }
    float Calculate_MoanCooldown(Actor* actor) {
        return MOAN_COOLDOWN * AnimationManager::GetAnimSpeed(actor);
    }
}

namespace GTS {

    CooldownManager& CooldownManager::GetSingleton() noexcept {
		static CooldownManager instance;
		return instance;
	}

	std::string CooldownManager::DebugName() {
		return "::CooldownManager";
	}

    CooldownData& CooldownManager::GetCooldownData(Actor* actor) {
		this->CooldownData.try_emplace(actor);
		return this->CooldownData.at(actor);
	}

    void CooldownManager::Reset() {
        this->CooldownData.clear();
        log::info("Cooldowns cleared");
    }

    void ApplyActionCooldown(Actor* giant, CooldownSource source) {
        double time = Time::WorldTimeElapsed();
        auto& data = CooldownManager::GetSingleton().GetCooldownData(giant);

        switch (source) {
            case CooldownSource::Damage_Launch: 
                data.lastLaunchTime = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Damage_Hand:
                data.lastHandDamageTime = Time::WorldTimeElapsed();
                break;    
            case CooldownSource::Damage_Thigh:
                data.lastThighDamageTime = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Push_Basic:
                data.lastPushTime = Time::WorldTimeElapsed();
                break;    
            case CooldownSource::Action_ButtCrush:
                data.lastButtCrushTime = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Action_HealthGate:
                data.lastHealthGateTime = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Action_ScareOther:   
                data.lastScareTime = Time::WorldTimeElapsed();
                break; 
            case CooldownSource::Action_Hugs:   
                data.lastHugTime = Time::WorldTimeElapsed();
                break;     
            case CooldownSource::Action_AbsorbOther:
                data.lastAbsorbTime = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Action_Breasts_Suffocate:
                data.lastBreastSuffocateTime = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Action_Breasts_Vore:
                data.lastBreastVoreTime = Time::WorldTimeElapsed();
                break;   
            case CooldownSource::Action_Breasts_Absorb:
                data.lastBreastAbsorbTime = Time::WorldTimeElapsed();
                break;        
            case CooldownSource::Emotion_Laugh:   
                data.lastLaughTime = Time::WorldTimeElapsed();
                break; 
            case CooldownSource::Emotion_Moan: 
                data.lastMoanTime = Time::WorldTimeElapsed();
                break;  
            case CooldownSource::Emotion_Moan_Crush: 
                data.lastMoanCrushTime = Time::WorldTimeElapsed();
                break;  
            case CooldownSource::Misc_RevertSound: 
                data.lastRevertTime = Time::WorldTimeElapsed();
                break; 
            case CooldownSource::Misc_GrowthSound:
                data.lastSoundGrowthTime = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Misc_BeingHit:
                data.lastHitTime = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Misc_AiGrowth:
                data.lastGrowthTime = Time::WorldTimeElapsed();
                break;    
            case CooldownSource::Misc_ShrinkOutburst:
                data.lastOutburstTime = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Misc_ShrinkOutburst_Forced:
                data.lastForceOutburstTime = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Misc_ShrinkParticle:
                data.lastShrinkParticleTime = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Misc_ShrinkParticle_Animation:
                data.lastAnimShrinkParticleTime = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Misc_ShrinkParticle_Gaze:
                data.lastGazeShrinkParticleTime = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Misc_TinyCalamityRage:
                data.lastTinyCalamityTime = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Footstep_Right:
                data.lastFootstepTime_R = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Footstep_Left:
                data.lastFootstepTime_L = Time::WorldTimeElapsed();
                break; 
            case CooldownSource::Footstep_JumpLand:
                data.lastJumplandTime = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Emotion_Voice:
                data.lastEmotionTime = Time::WorldTimeElapsed();
                break;
            case CooldownSource::Emotion_Voice_Long:
                data.lastEmotionTime_Long = Time::WorldTimeElapsed();
                break;
        }
    }

    double GetRemainingCooldown(Actor* giant, CooldownSource source) {
        double time = Time::WorldTimeElapsed();
        auto& data = CooldownManager::GetSingleton().GetCooldownData(giant);

        switch (source) {
            case CooldownSource::Damage_Launch: 
                return (data.lastLaunchTime + LAUNCH_COOLDOWN) - time;
            case CooldownSource::Damage_Hand:
                return (data.lastHandDamageTime + HANDDAMAGE_COOLDOWN) - time; 
            case CooldownSource::Damage_Thigh:
                return (data.lastThighDamageTime + THIGHDAMAGE_COOLDOWN) - time;
            case CooldownSource::Push_Basic:
                return (data.lastPushTime + PUSH_COOLDOWN) - time;
            case CooldownSource::Action_ButtCrush:
                return (data.lastButtCrushTime + Calculate_ButtCrushTimer(giant)) - time;
            case CooldownSource::Action_HealthGate:
                return (data.lastHealthGateTime + HEALTHGATE_COOLDOWN) - time;
            case CooldownSource::Action_ScareOther:   
                return time -(data.lastScareTime + SCARE_COOLDOWN) - time;
            case CooldownSource::Action_Hugs:
                return (data.lastHugTime + HUGS_COOLDOWN) - time;
            case CooldownSource::Action_AbsorbOther:
                return (data.lastAbsorbTime + Calculate_AbsorbCooldown(giant)) - time;    
            case CooldownSource::Action_Breasts_Suffocate:
                return (data.lastBreastSuffocateTime + Calculate_BreastActionCooldown(giant, 0)) - time;
            case CooldownSource::Action_Breasts_Vore:
                return (data.lastBreastVoreTime + Calculate_BreastActionCooldown(giant, 1)) - time;
            case CooldownSource::Action_Breasts_Absorb:
                return (data.lastBreastAbsorbTime + Calculate_BreastActionCooldown(giant, 2)) - time;
            case CooldownSource::Emotion_Laugh:   
                return (data.lastLaughTime + Calculate_LaughCooldown(giant)) - time;
            case CooldownSource::Emotion_Moan: 
                return (data.lastMoanTime + Calculate_MoanCooldown(giant)) - time;
            case CooldownSource::Emotion_Moan_Crush:
                return (data.lastMoanCrushTime + MOAN_CRUSH_COOLDOWN) - time;
            case CooldownSource::Misc_RevertSound: 
                return (data.lastRevertTime + SOUND_COOLDOWN) - time;
            case CooldownSource::Misc_GrowthSound:
                return (data.lastSoundGrowthTime + GROW_SOUND_COOLDOWN) - time;
            case CooldownSource::Misc_BeingHit:
                return (data.lastHitTime + HIT_COOLDOWN) - time;  
            case CooldownSource::Misc_AiGrowth:
                return (data.lastGrowthTime + AI_GROWTH_COOLDOWN) - time;  
            case CooldownSource::Misc_ShrinkOutburst:
                return (data.lastOutburstTime + Calculate_ShrinkOutbirstTimer(giant)) - time; 
            case CooldownSource::Misc_ShrinkOutburst_Forced:
                return (data.lastForceOutburstTime + SHRINK_OUTBURST_COOLDOWN_FORCED) - time; 
            case CooldownSource::Misc_ShrinkParticle:
                return (data.lastShrinkParticleTime + SHRINK_PARTICLE_COOLDOWN) - time;
            case CooldownSource::Misc_ShrinkParticle_Animation:
                return (data.lastAnimShrinkParticleTime + SHRINK_PARTICLE_COOLDOWN_ANIM) - time;
            case CooldownSource::Misc_ShrinkParticle_Gaze:
                return (data.lastGazeShrinkParticleTime + SHRINK_PARTICLE_COOLDOWN_GAZE) - time;
            case CooldownSource::Misc_TinyCalamityRage:
                return (data.lastTinyCalamityTime + SHRINK_TINYCALAMITY_RAGE) - time;
            case CooldownSource::Footstep_Right:
                return (data.lastFootstepTime_R + Calculate_FootstepTimer(giant)) - time;   
            case CooldownSource::Footstep_Left:
                return (data.lastFootstepTime_L + Calculate_FootstepTimer(giant)) - time; 
            case CooldownSource::Footstep_JumpLand:
                return (data.lastJumplandTime + Calculate_FootstepTimer(giant)) - time;
            case CooldownSource::Emotion_Voice:
                return (data.lastEmotionTime + Calculate_EmotionCooldown(giant)) - time;
            case CooldownSource::Emotion_Voice_Long:
                return (data.lastEmotionTime + Calculate_EmotionCooldown_Long(giant)) - time;
            }
        return 0.0;
    }

    bool IsActionOnCooldown(Actor* giant, CooldownSource source) {

        //Check the cleat flag to disable only action cooldowns and not others
        if (!Config::GetAdvanced().bCooldowns && Enum_Contains<CooldownSource>(source,"Action")) {
            return false;
        }

        double time = Time::WorldTimeElapsed();
        auto& data = CooldownManager::GetSingleton().GetCooldownData(giant);

        switch (source) {
            case CooldownSource::Damage_Launch: 
                return time <= (data.lastLaunchTime + LAUNCH_COOLDOWN);
            case CooldownSource::Damage_Hand:
                return time <= (data.lastHandDamageTime + HANDDAMAGE_COOLDOWN);
            case CooldownSource::Damage_Thigh:
                return time <= (data.lastThighDamageTime + THIGHDAMAGE_COOLDOWN);
            case CooldownSource::Push_Basic:
                return time <= (data.lastPushTime + PUSH_COOLDOWN);
            case CooldownSource::Action_ButtCrush:
                return time <= (data.lastButtCrushTime + Calculate_ButtCrushTimer(giant));
            case CooldownSource::Action_HealthGate:
                return time <= (data.lastHealthGateTime + HEALTHGATE_COOLDOWN);
            case CooldownSource::Action_ScareOther:   
                return time <= (data.lastScareTime + SCARE_COOLDOWN);
            case CooldownSource::Action_Breasts_Suffocate:
                return time <= (data.lastBreastSuffocateTime + Calculate_BreastActionCooldown(giant, 0));
            case CooldownSource::Action_Breasts_Vore:
                return time <= (data.lastBreastVoreTime + Calculate_BreastActionCooldown(giant, 1));
            case CooldownSource::Action_Breasts_Absorb:
                return time <= (data.lastBreastAbsorbTime + Calculate_BreastActionCooldown(giant, 2));
            case CooldownSource::Action_Hugs:
                return time <= (data.lastHugTime + HUGS_COOLDOWN);
            case CooldownSource::Action_AbsorbOther:
                return time <= (data.lastAbsorbTime + Calculate_AbsorbCooldown(giant));
            case CooldownSource::Emotion_Laugh:   
                return time <= (data.lastLaughTime + LAUGH_COOLDOWN);
            case CooldownSource::Emotion_Moan: 
                return time <= (data.lastMoanTime + MOAN_COOLDOWN);
            case CooldownSource::Emotion_Moan_Crush:
                return time <= (data.lastMoanCrushTime + MOAN_CRUSH_COOLDOWN);
            case CooldownSource::Misc_RevertSound: 
                return time <= (data.lastRevertTime + SOUND_COOLDOWN);
            case CooldownSource::Misc_GrowthSound:
                return time <= (data.lastSoundGrowthTime + GROW_SOUND_COOLDOWN);
            case CooldownSource::Misc_BeingHit:
                return time <= (data.lastHitTime + HIT_COOLDOWN);  
            case CooldownSource::Misc_AiGrowth:
                return time <= (data.lastGrowthTime + AI_GROWTH_COOLDOWN);
            case CooldownSource::Misc_ShrinkOutburst:
                return time <= (data.lastOutburstTime + Calculate_ShrinkOutbirstTimer(giant));   
            case CooldownSource::Misc_ShrinkOutburst_Forced:
                return time <= (data.lastForceOutburstTime + SHRINK_OUTBURST_COOLDOWN_FORCED);   
            case CooldownSource::Misc_ShrinkParticle:
                return time <= (data.lastShrinkParticleTime + SHRINK_PARTICLE_COOLDOWN); 
            case CooldownSource::Misc_ShrinkParticle_Animation:
                return time <= (data.lastAnimShrinkParticleTime + SHRINK_PARTICLE_COOLDOWN_ANIM);
            case CooldownSource::Misc_ShrinkParticle_Gaze:
                return time <= (data.lastGazeShrinkParticleTime + SHRINK_PARTICLE_COOLDOWN_GAZE);
            case CooldownSource::Misc_TinyCalamityRage:
                return time <= (data.lastTinyCalamityTime + SHRINK_TINYCALAMITY_RAGE); 
            case CooldownSource::Footstep_Right:
                return time <= (data.lastFootstepTime_R + Calculate_FootstepTimer(giant));    
            case CooldownSource::Footstep_Left:
                return time <= (data.lastFootstepTime_L + Calculate_FootstepTimer(giant));  
            case CooldownSource::Footstep_JumpLand:
                return time <= (data.lastJumplandTime + Calculate_FootstepTimer(giant));
            case CooldownSource::Emotion_Voice:
                return time <= (data.lastEmotionTime + Calculate_EmotionCooldown(giant));
            case CooldownSource::Emotion_Voice_Long:
                return time <= (data.lastEmotionTime_Long + Calculate_EmotionCooldown_Long(giant));
            }
        return false; 
    }
}