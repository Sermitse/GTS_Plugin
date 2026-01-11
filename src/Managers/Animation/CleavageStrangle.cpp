#include "Managers/Animation/CleavageStrangle.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Grab.hpp"
#include "Managers/GTSSizeManager.hpp"
#include "Managers/Rumble.hpp"
#include "Magic/Effects/Common.hpp"
#include "Utils/DifficultyUtils.hpp"

using namespace GTS;

namespace {

    const std::vector<std::string_view> Nodes_3BBB = { "L Breast03","R Breast03", };
    const std::vector<std::string_view> Nodes_1BBB = { "NPC L Breast","NPC R Breast", };

    void ThresholdReachedRumbling(Actor* giant) {
        auto get_node = find_node(giant, "L Breast03");

        if (get_node) {
            for (auto node : Nodes_3BBB) {
                Rumbling::Once("ReadyToAbsorb1", giant, 4.2f, 0.075f, node, true);
            }
        }
        else {
            for (auto node : Nodes_1BBB) {
                Rumbling::Once("ReadyToAbsorb2", giant, 4.2f, 0.075f, node, true);
            }
        }
    }

    void Task_SpawnHeartsOverTime(Actor* giant, Actor* tiny, std::string_view task_name) {
        ActorHandle gianthandle = giant->CreateRefHandle();
        ActorHandle tinyhandle = tiny->CreateRefHandle();
        static Timer HeartTimer = Timer(1.75f);

        TaskManager::Run(task_name, [=](auto& progressData) {
            if (!gianthandle) {
                return false;
            }
            if (!tinyhandle) {
                return false;
            }
            auto giantref = gianthandle.get().get();
            auto tinyref = tinyhandle.get().get();

            if (!tinyref) {
                return false; // end task in that case
            }

            if (!AnimationVars::Cleavage::GetIsBoobsDoting(giantref)) {
                return false;
            }

            /*if (!IsGtsBusy(tinyref)) {
                AnimationManager::StartAnim("Cleavage_Absorb_Tiny", tinyref);
                // Hide the Tiny by making tiny roll into a ball
            }*/

            if (!IsGtsBusy(tinyref)) { // If for some reason Tiny isn't in expected anim
                AnimationManager::StartAnim("Cleavage_DOT_Start_Tiny", tinyref);
            }
            
            if (HeartTimer.ShouldRunFrame()) {
                SpawnHearts(giantref, tinyref, 35.0f, 0.425f, false);
            }

            // All good try another frame
            return true;
            });
    }
    void ResetAnimationData(AnimationEventData& data, bool manager = false) {
        data.animSpeed = 1.0f;
        data.canEditAnimSpeed = false;
        data.stage = 0;
    }

    void Task_SlowlyDamageTiny(Actor* giant, Actor* tiny, std::string_view task_name) {
        ActorHandle gianthandle = giant->CreateRefHandle();
        ActorHandle tinyhandle = tiny->CreateRefHandle();

        auto data = Transient::GetActorData(tiny);
        float damage_Setting = GetDifficultyMultiplier(giant, tiny);
        float threshold = 0.075f;

        if (!IsTeammate(tiny)) {
            tiny->StartCombat(giant);
            tiny->Attacked(giant); // force combat
        }

        TaskManager::Run(task_name, [=](auto& progressData) {
            if (!gianthandle) {
                return false;
            }
            if (!tinyhandle) {
                return false;
            }
            auto giantref = gianthandle.get().get();
            auto tinyref = tinyhandle.get().get();

            if (!tinyref) {
                return false; // end task in that case
            }

            float max_tiny_hp = GetMaxAV(tinyref, ActorValue::kHealth) * threshold;
            float tiny_health_perc = GetHealthPercentage(tinyref);
            float tiny_health = GetAV(tinyref, ActorValue::kHealth);

            auto& sizemanager = SizeManager::GetSingleton();

            float power = std::clamp(SizeManager::GetSizeAttribute(giantref, SizeAttribute::Normal), 1.0f, 999999.0f);
            float sizeDiff = get_scale_difference(giantref, tinyref, SizeType::VisualScale, false, false);
            float additionaldamage = 1.0f + sizemanager.GetSizeVulnerability(tinyref);
            float speed = AnimationManager::GetBonusAnimationSpeed(giantref);

            float damage = Damage_Breast_Strangle * power * additionaldamage * sizeDiff * TimeScale() * speed;
            HasSMT(giantref) ? damage *= 1.5f : damage *= 1.0f;

            if (!AnimationVars::Cleavage::GetIsBoobsDoting(giantref) && !IsGtsBusy(giantref)) {
                AnimationManager::ResetAnimationSpeedData(giantref);
                RestoreBreastAttachmentState(giantref, tinyref); // If someone suddenly ragdolls us during breast anims
                return false;
            }

            if (tiny_health_perc <= threshold || tiny_health - (damage * damage_Setting) <= max_tiny_hp) {
                if (data) {
                    if (data->ImmuneToBreastOneShot) {
                        AnimationManager::StartAnim("Cleavage_DOT_Stop", giantref); // Spare tiny, return to idle breast loop
                        AnimationManager::StartAnim("Cleavage_DOT_Stop_Tiny", tinyref);
                        SpawnHearts(giantref, tinyref, 35.0f, 0.6f, false);
                        ThresholdReachedRumbling(giantref);

                        data->ImmuneToBreastOneShot = false; // Tiny is no longer protected, kill tiny next time instead
                        return false;
                    }
                    else if (!data->ImmuneToBreastOneShot && tiny_health - (damage * damage_Setting * 2.0f) <= 2.5f || tinyref->IsDead()) {
                        AnimationManager::StartAnim("Cleavage_DOT_Kill", giantref); // play finisher
                        AnimationManager::StartAnim("Cleavage_DOT_Kill_Tiny", tinyref);
                        return false;
                    }
                }
            }

            InflictSizeDamage(giantref, tinyref, damage);

            if (tinyref->IsDead() || giantref->IsDead() || GetAV(tinyref, ActorValue::kHealth) <= 0.0f) {
                AnimationManager::ResetAnimationSpeedData(giantref);
                return false;
            }
            // All good try another frame
            return true;
        });
    }

    void GTS_BS_StartDOT(AnimationEventData& data) {
        auto tiny = Grab::GetHeldActor(&data.giant);
        Actor* giant = &data.giant;
        if (tiny) {
            std::string_view name = std::format("Suffo_DOT_{}_{}", giant->formID, tiny->formID);
            std::string_view name_2 = std::format("Hearts_DOT_{}_{}", giant->formID, tiny->formID);
            Task_SpawnHeartsOverTime(&data.giant, tiny, name_2);
            Task_SlowlyDamageTiny(&data.giant, tiny, name);
        }
        data.stage = 1;
        data.canEditAnimSpeed = true;
    }
    void GTS_BS_StopDOT(AnimationEventData& data) { // fail-safe
        auto tiny = Grab::GetHeldActor(&data.giant);
        Actor* giant = &data.giant;
        if (tiny) {
            std::string_view name = std::format("Suffo_DOT_{}_{}", giant->formID, tiny->formID);
            AnimationManager::StartAnim("Cleavage_DOT_Leave_Tiny", tiny);
            TaskManager::Cancel(name);
        }
        ResetAnimationData(data);
    }

    void GTS_BS_Smile(const AnimationEventData& data) {
        if (AnimationVars::Cleavage::GetIsBoobsDoting(&data.giant)) { // This event belongs to Breast Vore originally, but time to smile is like 0.2 sec so it's unused, and used here instead
            if (RandomBool(65.0f)) {
                Task_FacialEmotionTask_SlightSmile(&data.giant, RandomFloat(3.75f, 5.5f), "StrangleSmile", RandomFloat(0.35f, 0.75f));
            }
            else {
                Task_FacialEmotionTask_Smile(&data.giant, RandomFloat(3.75f, 5.5f), "StrangleSmile", RandomFloat(0.35f, 0.75f));
            }
        }
    }
}
namespace GTS {

    void Animation_CleavageStrangle::RegisterEvents() {
        AnimationManager::RegisterEvent("GTS_BS_Smile", "Cleavage", GTS_BS_Smile);
        AnimationManager::RegisterEvent("GTS_BS_StartDOT", "Cleavage", GTS_BS_StartDOT);
        AnimationManager::RegisterEvent("GTS_BS_StopDOT", "Cleavage", GTS_BS_StopDOT);
        // Input function is registered inside CleavageState.cpp as "CleavageDOT" since function that starts the anim is there
    }

    void Animation_CleavageStrangle::RegisterTriggers() {
        AnimationManager::RegisterTrigger("Cleavage_DOT_Start", "Cleavage", "GTSBEH_Boobs_Crush_Dot");
        AnimationManager::RegisterTrigger("Cleavage_DOT_Stop", "Cleavage", "GTSBEH_Boobs_Crush_Dot_Stop");
        AnimationManager::RegisterTrigger("Cleavage_DOT_Kill", "Cleavage", "GTSBEH_Boobs_Crush_Kill");

        AnimationManager::RegisterTrigger("Cleavage_DOT_Start_Tiny", "Cleavage", "GTSBEH_T_Boobs_Crush_Dot");
        AnimationManager::RegisterTrigger("Cleavage_DOT_Stop_Tiny", "Cleavage", "GTSBEH_T_Boobs_Crush_Dot_Stop");
        AnimationManager::RegisterTrigger("Cleavage_DOT_Kill_Tiny", "Cleavage", "GTSBEH_T_Boobs_Crush_Kill");

        AnimationManager::RegisterTrigger("Cleavage_DOT_Leave_Tiny", "Cleavage", "GTSBEH_T_BS_DOT_Leave");
    }
}