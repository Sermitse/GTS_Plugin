#include "Hooks/Actor/Detection.hpp"
#include "Managers/Animation/Grab.hpp"
#include "Hooks/Util/HookUtil.hpp"

using namespace GTS;

namespace {

    float modify_detection(float in) {
        float modify = 1.0f;
        if (in > 1e-6) {
            auto player = PlayerCharacter::GetSingleton();
            float scale = get_visual_scale(player);
            modify = 1.0f / scale;
        }
        return modify;
    }

    float modify_footstep_detection(Actor* giant, float in) {
        float scale = get_visual_scale(giant);
        float massscale = (scale * scale * scale);
        float modify = 0.0f;
        if (TinyCalamityActive(giant)) {
            modify = (in * 4.0f + 80) * scale;
        }
        else {
            if (in > 1e-6) {
                modify = in * massscale;
            }
            else {
                modify = (1.0f * massscale) - 1.0f;
            }
        }

        if (scale < get_natural_scale(giant)) {
            modify += in * massscale;
            modify -= 1.0f / massscale; // harder to hear small player
        }

        return modify;
    }

}


namespace Hooks {

    struct CalculateFootStepDetection {

        // SE:
        //  0x1405FD870 : 36758
        //  0x1405fdb44 - 0x1405FD870 = 0x2d4 
        //  altering Character::GetEquippedWeight_1406195D0
        // AE:
        //  0x140625520 : 37774
        //  0x1406257f2 - 0x140625520 = 0x2D2


        static float thunk(Actor* a_this) {

            float result = func(a_this); // Makes footsteps lounder for AI

            {
                GTS_PROFILE_ENTRYPOINT("ActorDetection::CalculateFootStepDetection");

                if (a_this->IsPlayerRef() || IsTeammate(a_this)) {
                    //log::info("Hook Weight Result for {} is {}", giant->GetDisplayFullName(), result);
                    float alter = modify_footstep_detection(a_this, result);
                    result = alter;
                }
            }

            return result;

        }

		FUNCTYPE_CALL func;

    };

    struct CalculateHeading1 {
        // 2026 Note: Disabled.
        //      It's unclear how exactly it works, better rely on altering Value + Altering Footstep volume
        // SE:
        //  0x1405FD870 : 36758
        //  0x1405fda87 - 0x1405FD870 = 0x217 (line ~150)
        //  altering Character::GetHeading_1405FD780
        // AE:
        //  0x140625520 : 37774
        //  0x140625737 - 0x140625520 = 0x217 (wow same rel)

        static float thunk(Actor* a_this, NiPoint3* param_1) {

            float result = func(a_this, param_1);

            {
                GTS_PROFILE_ENTRYPOINT("ActorDetection::CalculateHeading1");
                result *= modify_detection(result);
            }

            return result;
        }

        FUNCTYPE_CALL func;

    };

    struct CalculateHeading2 {
        // 2026 Note: Disabled.
        //      It's unclear how exactly it works, better rely on altering Value + Altering Footstep volume
        //  0x1405FD870 : 36758
        //  0x1405fe19d - 0x1405FD870 = 0x92D (line 370)
        //  altering Character::GetHeading_1405FD780
        // AE:
        //  0x140625520 : 37774
        //  0x140625f9f - 0x140625520 = 0xA7F

        static float thunk(Actor* a_this, NiPoint3* param_1) {

            float result = func(a_this, param_1);

            {
                GTS_PROFILE_ENTRYPOINT("ActorDetection::CalculateHeading2");
                result *= modify_detection(result);
            }

            return result;
        }

        FUNCTYPE_CALL func;

    };

	struct DoDetectionJob_CalculateDetection {

		static uint8_t* thunk(Actor* a_observer,   // param_1
            Actor* target,                         // param_2
            int* detScore,                         // param_3 - the detection score output
            bool* inCone,                          // param_4
            bool* hasLOS,                          // param_5
            int* losResult,                        // param_6
            float* triggerPos,                     // param_7 - float[3], xyz of what triggered detection
            uint64_t p8,                           // param_8 - unknown, passed straight into Character::sub
            uint64_t p9,                           // param_9 - unknown, passed straight into Character::sub
            uint64_t p10                           // param_10 - unknown, passed straight into Character::sub) {
        ){
            {
                GTS_PROFILE_ENTRYPOINT("ActorDetection::DoDetectionJob");

                if (a_observer) {
                    if (IsHuman(a_observer)) {
                        //Basically makes the holder blind to other npcs
                        //or the target if the target is being hugged
                        const bool isTargetBeingHugged = GTS::AnimationVars::Tiny::IsBeingHugged(target) || GTS::AnimationVars::Tiny::IsBeingCrawlHugged(target);
                        const bool isObserverGrabbing = Grab::GetHeldActor(a_observer);
                        const bool shouldNotBeDetected = isTargetBeingHugged || isObserverGrabbing;
                       
                    	if (shouldNotBeDetected) {
                            *detScore = -1000;
                            *inCone = false;
                            *hasLOS = false;
                            *losResult = 0;
                            return nullptr;
                        }
                    }
                }
            }
			return func(a_observer, target, detScore, inCone, hasLOS, losResult, triggerPos, p8, p9, p10);
		}

        FUNCTYPE_CALL func;

	};

    struct Character_CalculateDetectionStrength {

        //Unless this is secretly a struct in disguise wtf were they thinking when making the func...
    	//80% of the function arg names are probably wrong...
        //Not like it matters since we scale its output anyways.

        static int32_t thunk(
            Actor* observer,        // param_1
            Actor* target,          // param_2
            float observerSneak,    // param_3 - sneak skill, scaled if player
            float targetSneak,      // param_4 - target sneak skill, scaled if player, perk modified
            int losResultCode,      // param_5 - 0/1/2/3 , indexes into fVar10 lookup table
            int hasLOS,             // param_6 - < 1 dampens distance score if not alert
            int lightLevel,         // param_7 - used in noise component if param_11 != 0
            float distance,         // param_8 - distance to target
            int lightLevelAdjusted, // param_9 - light level after ambient subtraction, passed into noise formula
            int observerSneakSkill, // param_10 - (100 - this) * 0.01 = noise scalar
            char hasLOS2,           // param_11 - gates lightLevel into noise component
            float targetScale,      // param_12 - actor scale float, used in distance component
            byte isRunning,         // param_13 - contributes to distance component
            byte isSneaking,        // param_14 - subtracted from misc modifier via local_res20
            float equippedWeight,   // param_15 - contributes to distance component, 0 floored
            byte hasMuffle,         // param_16 - toggles DAT_141ffb808 multiplier in distance, perk 0x30
            byte isWalking,         // param_17 - gates noise component, gates fVar8 in distance
            byte isInCombat,        // param_18 - contributes to misc via _DAT_141ffb970
            int raceSize,           // param_19 - race size enum, linear scale in distance component
            char isIndoors,         // param_20 - scales max range and switches angle weights
            byte isCombatTarget,    // param_21 - contributes to misc via _DAT_141ffc6c0
            int alertLevel,         // param_22 - 0/1/2 switches DAT values, 1 disables LOS dampening
            float* outVisibility,   // param_23 - written with distance/visibility component
            float* outNoise,        // param_24 - written with noise/movement component
            float* outMisc){        // param_25 - written with misc modifier component

            int32_t result = func(
                observer, target,
                observerSneak, targetSneak,
                losResultCode, hasLOS, lightLevel,
                distance, lightLevelAdjusted, observerSneakSkill,
                hasLOS2, targetScale,
                isRunning, isSneaking, equippedWeight,
                hasMuffle, isWalking, isInCombat,
                raceSize, isIndoors, isCombatTarget, alertLevel,
                outVisibility, outNoise, outMisc
            );

            {
                GTS_PROFILE_ENTRYPOINT("ActorDetection::CalculateDetectionStrength");

                if (result <= 0) return result;

                { // scale by observer size: larger observer detects more easily
                    float observerVisualScale = get_visual_scale(observer);
                    result = static_cast<int32_t>(std::floorf(result * observerVisualScale));
                }

                { // scale by target size: larger target is easier to detect, smaller harder
                    float targetVisualScale = get_visual_scale(target);
                    result = static_cast<int32_t>(std::floorf(result / targetVisualScale));
                }

                return std::clamp(result, 0, 100);
            }

        }

        FUNCTYPE_CALL func;

    };

	void Hook_Detection::Install() {

		logger::info("Installing Detection Hooks...");

        stl::write_call<CalculateFootStepDetection>(REL::RelocationID(36758, 37774, NULL), REL::VariantOffset(0x2D4, 0x2D2, NULL));
        //stl::write_call<CalculateHeading1>(REL::RelocationID(36758, 37774, NULL), REL::VariantOffset(0x217, 0x217, NULL));
        //stl::write_call<CalculateHeading2>(REL::RelocationID(36758, 37774, NULL), REL::VariantOffset(0x92D, 0xA7F, NULL));
		stl::write_call<DoDetectionJob_CalculateDetection>(REL::RelocationID(41659, 42742, NULL), REL::VariantOffset(0x526, 0x67B, NULL));
        //stl::write_call<Character_CalculateDetectionStrength>(REL::RelocationID(36758, 37774, NULL), REL::VariantOffset(0x5c7, 0x5b9, NULL));

	}
}
