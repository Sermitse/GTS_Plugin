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
        if (HasSMT(giant)) {
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

            GTS_PROFILE_ENTRYPOINT("ActorDetection::CalculateFootStepDetection");

            float result = func(a_this); // Makes footsteps lounder for AI
            if (a_this->formID == 0x14 || IsTeammate(a_this)) {
                //log::info("Hook Weight Result for {} is {}", giant->GetDisplayFullName(), result);
                float alter = modify_footstep_detection(a_this, result);
                result = alter;
            }
            return result;

        }

		FUNCTYPE_CALL func;

    };

    struct CalculateHeading1 {

        // SE:
        //  0x1405FD870 : 36758
        //  0x1405fda87 - 0x1405FD870 = 0x217 (line ~150)
        //  altering Character::GetHeading_1405FD780
        // AE:
        //  0x140625520 : 37774
        //  0x140625737 - 0x140625520 = 0x217 (wow same rel)

        static float thunk(Actor* a_this, NiPoint3* param_1) {

            GTS_PROFILE_ENTRYPOINT("ActorDetection::CalculateHeading1");

            float result = func(a_this, param_1);
            result *= modify_detection(result);
            return result;
        }

        FUNCTYPE_CALL func;

    };

    struct CalculateHeading2 {

        // SE:
        //  0x1405FD870 : 36758
        //  0x1405fe19d - 0x1405FD870 = 0x92D (line 370)
        //  altering Character::GetHeading_1405FD780
        // AE:
        //  0x140625520 : 37774
        //  0x140625f9f - 0x140625520 = 0xA7F

        static float thunk(Actor* a_this, NiPoint3* param_1) {

            GTS_PROFILE_ENTRYPOINT("ActorDetection::CalculateHeading2");

            float result = func(a_this, param_1);
            result *= modify_detection(result);
            return result;
        }

        FUNCTYPE_CALL func;

    };

	struct DoDetectionJob {

		static uint8_t* thunk(RE::Actor* a_source, RE::Actor* a_target, std::int32_t& a_detectionValue, std::uint8_t& a_unk04, std::uint8_t& a_unk05, std::uint32_t& a_unk06, RE::NiPoint3& a_pos, float& a_unk08, float& a_unk09, float& a_unk10) {

            GTS_PROFILE_ENTRYPOINT("ActorDetection::DoDetectionJob");

			if (a_source) {

				if (IsHuman(a_source)) {
					if (Grab::GetHeldActor(a_source)) {
						a_detectionValue = -1000;
						return nullptr;
					}
				}
			}

			//Doesn't seem to work
			/*if (a_target) {
				//Scale the detection value based on the target's scale
				a_detectionValue = static_cast<int32_t>(static_cast<float>(a_detectionValue) * get_visual_scale(a_target));
			}*/

			return func(a_source, a_target, a_detectionValue, a_unk04, a_unk05, a_unk06, a_pos, a_unk08, a_unk09, a_unk10);
		}

        FUNCTYPE_CALL func;

	};

	void Hook_Detection::Install() {

		logger::info("Installing Detection Hooks...");

        stl::write_call<CalculateFootStepDetection>(REL::RelocationID(36758, 37774, NULL), REL::VariantOffset(0x2D4, 0x2D2, NULL));
        stl::write_call<CalculateHeading1>(REL::RelocationID(36758, 37774, NULL), REL::VariantOffset(0x217, 0x217, NULL));
        stl::write_call<CalculateHeading2>(REL::RelocationID(36758, 37774, NULL), REL::VariantOffset(0x92D, 0xA7F, NULL));
		stl::write_call<DoDetectionJob>(REL::RelocationID(41659, 42742, NULL), REL::VariantOffset(0x526, 0x67B, NULL));
	}
}
