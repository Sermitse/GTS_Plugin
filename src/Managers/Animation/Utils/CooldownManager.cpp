#include "Managers/Animation/Utils/CooldownManager.hpp"
#include "Constants.hpp"

#include "Config/Config.hpp"

#include "Managers/Animation/AnimationManager.hpp"

using namespace GTS;

namespace {
    float Calculate_FootstepTimer(Actor* actor) {
        float cooldown = 0.2f;
        cooldown /= AnimationManager::GetAnimSpeed(actor);
        return cooldown;
    }

    float Calculate_EmotionCooldown(Actor* actor) {
        return EMOTION_COOLDOWN / AnimationManager::GetAnimSpeed(actor);
    }
    float Calculate_EmotionCooldown_Long(Actor* actor) {
        return EMOTION_COOLDOWN_LONG / AnimationManager::GetAnimSpeed(actor);
    }
    float Calculate_LaughCooldown(Actor* actor) {
        return LAUGH_COOLDOWN / AnimationManager::GetAnimSpeed(actor);
    }
    float Calculate_MoanCooldown(Actor* actor) {
        return MOAN_COOLDOWN / AnimationManager::GetAnimSpeed(actor);
    }
    float Calculate_CalamityShrinkCooldown(Actor* actor) {
        return actor->IsPlayerRef() ? TINYCALAMITY_SHRINK_COOLDOWN : TINYCALAMITY_SHRINK_COOLDOWN_NPC;
    }
}

namespace GTS {

    float Calculate_ShrinkOutburstTimer(Actor* actor) {
        bool DarkArts3 = Runtime::HasPerk(actor, Runtime::PERK.GTSPerkDarkArtsAug3);
        bool HealthRegen = Runtime::HasPerk(actor, Runtime::PERK.GTSPerkGrowthAug2);
        bool DarkArts_Legendary = Runtime::HasPerk(actor, Runtime::PERK.GTSPerkDarkArtsLegendary);
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

    float Calculate_BreastActionCooldown(Actor* giant, int type) {
        float Cooldown = 1.0f;
        float mastery = 0.0f;
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

        if (Runtime::HasPerk(giant, Runtime::PERK.GTSPerkBreastsMastery1)) {
            float level = GetGtsSkillLevel(giant) - 40.0f; // Start past level 40
            mastery = std::clamp(level * 0.01f, 0.0f, 0.6f);
        }
        float reduction = 1.0f - mastery;

        return Cooldown * reduction;
    }

    float Calculate_HugCrushCooldown(Actor* giant) {
        float mastery = std::clamp(GetGtsSkillLevel(giant) * 0.01f, 0.0f, 1.0f) * 0.666f;
        float reduction = 1.0f - mastery; // Up to 15 seconds at level 100

        return ABSORB_OTHER_COOLDOWN * reduction;
    }

    float Calculate_ButtCrushTimer(Actor* actor) {
        bool lvl70 = Runtime::HasPerk(actor, Runtime::PERK.GTSPerkButtCrushAug3);
        bool lvl100 = Runtime::HasPerk(actor, Runtime::PERK.GTSPerkButtCrushAug4);
        float reduction = 1.0f;
        if (lvl100) { // 15% reduction
            reduction -= 0.15f;
        }
        if (lvl70) { // 10% reduction
            reduction -= 0.10f;
        }
        return BUTTCRUSH_COOLDOWN * reduction;
    }
}

namespace {
    using namespace GTS;
    using CooldownTable = std::array<CooldownConfig, static_cast<size_t>(CooldownSource::Total)>;

    CooldownTable BuildCooldownTable() {
        CooldownTable table{};
        auto set = [&](CooldownSource source, CooldownConfig config) {
            table[static_cast<size_t>(source)] = std::move(config);
        };

        set(CooldownSource::Damage_Launch,                      { .staticCooldown = LAUNCH_COOLDOWN });
        set(CooldownSource::Damage_Hand,                        { .staticCooldown = HANDDAMAGE_COOLDOWN });
        set(CooldownSource::Damage_Thigh,                       { .staticCooldown = THIGHDAMAGE_COOLDOWN });
        set(CooldownSource::Push_Basic,                         { .staticCooldown = PUSH_COOLDOWN });
        set(CooldownSource::Action_ButtCrush,                   { .dynamicCooldown = Calculate_ButtCrushTimer });
        set(CooldownSource::Action_HealthGate,                  { .staticCooldown = HEALTHGATE_COOLDOWN });
        set(CooldownSource::Action_ScareOther,                  { .staticCooldown = SCARE_COOLDOWN });
        set(CooldownSource::Action_HugAbsorbOther,              { .dynamicCooldown = Calculate_HugCrushCooldown });
        set(CooldownSource::Action_Breasts_Absorb,              { .dynamicCooldown = [](Actor* a) { return Calculate_BreastActionCooldown(a, 2); } });
        set(CooldownSource::Action_Breasts_Suffocate,           { .dynamicCooldown = [](Actor* a) { return Calculate_BreastActionCooldown(a, 0); } });
        set(CooldownSource::Action_Breasts_Vore,                { .dynamicCooldown = [](Actor* a) { return Calculate_BreastActionCooldown(a, 1); } });
        set(CooldownSource::Action_Hugs,                        { .staticCooldown = HUGS_COOLDOWN });
        set(CooldownSource::Emotion_Laugh,                      { .dynamicCooldown = Calculate_LaughCooldown });
        set(CooldownSource::Emotion_Moan,                       { .dynamicCooldown = Calculate_MoanCooldown });
        set(CooldownSource::Emotion_Moan_Crush,                 { .staticCooldown = MOAN_CRUSH_COOLDOWN });
        set(CooldownSource::Misc_RevertSound,                   { .staticCooldown = SOUND_COOLDOWN });
        set(CooldownSource::Misc_GrowthSound,                   { .staticCooldown = GROW_SOUND_COOLDOWN });
        set(CooldownSource::Misc_BeingHit,                      { .staticCooldown = HIT_COOLDOWN });
        set(CooldownSource::Misc_AiGrowth,                      { .staticCooldown = AI_GROWTH_COOLDOWN });
        set(CooldownSource::Misc_ShrinkOutburst,                { .dynamicCooldown = Calculate_ShrinkOutburstTimer });
        set(CooldownSource::Misc_ShrinkOutburst_Forced,         { .staticCooldown = SHRINK_OUTBURST_COOLDOWN_FORCED });
        set(CooldownSource::Misc_ShrinkParticle,                { .staticCooldown = SHRINK_PARTICLE_COOLDOWN });
        set(CooldownSource::Misc_ShrinkParticle_Animation,      { .staticCooldown = SHRINK_PARTICLE_COOLDOWN_ANIM });
        set(CooldownSource::Misc_ShrinkParticle_Gaze,           { .staticCooldown = SHRINK_PARTICLE_COOLDOWN_GAZE });
        set(CooldownSource::Misc_TinyCalamity_WrathfulCalamity, { .staticCooldown = TINYCALAMITY_ONESHOT_COOLDOWN });
        set(CooldownSource::Misc_TinyCalamity_Shrink,           { .dynamicCooldown = Calculate_CalamityShrinkCooldown });
        set(CooldownSource::Misc_TinyCalamity_Hit,              { .staticCooldown = TINYCALAMITY_MELEEHIT_PUSH });
        set(CooldownSource::Misc_TinyCalamity_RunPushAway,      { .staticCooldown = TINYCALAMITY_RUN_PUSHAWAY });
        set(CooldownSource::Misc_TinyCalamity_Ragdoll,          { .staticCooldown = TINYCALAMITY_MELEEHIT_RAGDOLL });
        set(CooldownSource::Footstep_Right,                     { .dynamicCooldown = Calculate_FootstepTimer });
        set(CooldownSource::Footstep_Left,                      { .dynamicCooldown = Calculate_FootstepTimer });
        set(CooldownSource::Footstep_JumpLand,                  { .dynamicCooldown = Calculate_FootstepTimer });
        set(CooldownSource::Emotion_Voice,                      { .dynamicCooldown = Calculate_EmotionCooldown });
        set(CooldownSource::Emotion_Voice_Long,                 { .dynamicCooldown = Calculate_EmotionCooldown_Long });

        return table;
    }
}

namespace GTS {

    const CooldownConfig& GetCooldownConfig(CooldownSource source) {
        static const CooldownTable table = BuildCooldownTable();
        return table[static_cast<size_t>(source)];
    }

    double& CooldownManager::GetLastTime(Actor* actor, CooldownSource source) {
        size_t idx = static_cast<size_t>(source);
        if (_lastAccessedIndex >= 0 && static_cast<size_t>(_lastAccessedIndex) < _lastActionTimes.size()
            && _lastActionTimes[_lastAccessedIndex].actor == actor) {
            return _lastActionTimes[_lastAccessedIndex].times[idx];
        }

        for (size_t i = 0; i < _lastActionTimes.size(); ++i) {
            if (_lastActionTimes[i].actor == actor) {
                _lastAccessedIndex = static_cast<int>(i);
                return _lastActionTimes[i].times[idx];
            }
        }

        // Fresh actor - defaults every source to "never triggered", same
        // intent as the old struct's -1.0e8 field defaults.
        _lastActionTimes.push_back({});
        ActorCooldowns& entry = _lastActionTimes.back();
        entry.actor = actor;
        entry.times.fill(-1.0e8);

        _lastAccessedIndex = static_cast<int>(_lastActionTimes.size() - 1);
        return entry.times[idx];
    }

    void ApplyActionCooldown(Actor* giant, CooldownSource source) {
        if (!giant) {
            return;
        }
        CooldownManager::GetSingleton().GetLastTime(giant, source) = Time::WorldTimeElapsed();
    }

    float GetRemainingCooldown(Actor* giant, CooldownSource source) {
        if (!giant) {
            return 0.0;
        }
        double last = CooldownManager::GetSingleton().GetLastTime(giant, source);
        double duration = GetCooldownConfig(source).Get(giant);
        const float cooldown = static_cast<float>((last + duration) - Time::WorldTimeElapsed());
        return cooldown;
    }

    bool IsActionOnCooldown(Actor* giant, CooldownSource source) {
        // Keep the global cooldowns-disabled toggle for Action_*/*_TinyCalamity_* sources.
        if (!Config::Advanced.bCooldowns && (Enum_Contains<CooldownSource>(source, "Action") || Enum_Contains<CooldownSource>(source, "TinyCalamity"))) {
            return false;
        }
        return GetRemainingCooldown(giant, source) >= 0.0f;
    }

    std::string CooldownManager::DebugName() {
        return "::CooldownManager";
    }

    void CooldownManager::Reset() {
        _lastActionTimes.clear();
        _lastAccessedIndex = -1;
        logger::info("Cooldowns cleared");
    }
}