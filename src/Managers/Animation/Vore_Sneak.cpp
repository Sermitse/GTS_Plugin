#include "Managers/Animation/Vore_Sneak.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Controllers/VoreController.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"

using namespace GTS;

namespace {
    void GTS_Sneak_Vore_Start(AnimationEventData& data) {
        auto giant = &data.giant;
		auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);
		VoreData.AllowToBeVored(false);
		for (auto& tiny: VoreData.GetVories()) {
			AllowToBeCrushed(tiny, false);
			DisableCollisions(tiny, giant);
            SetBeingHeld(tiny, true);
		}

		Task_HighHeel_SyncVoreAnim(giant);
    }

    void GTS_Sneak_Vore_Grab(AnimationEventData& data) {
		auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);
		for (auto& tiny: VoreData.GetVories()) {
			if (!Vore_ShouldAttachToRHand(&data.giant, tiny)) {
				VoreData.GrabAll();
			}
			tiny->NotifyAnimationGraph("JumpFall");
			Attacked(tiny, &data.giant);
		}
		if (IsTransferingTiny(&data.giant)) {
			ManageCamera(&data.giant, true, CameraTracking::ObjectA);
		} else {
			ManageCamera(&data.giant, true, CameraTracking::Hand_Right);
		}
    }

    void GTS_Sneak_Vore_OpenMouth(AnimationEventData& data) {
		auto giant = &data.giant;
		auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);

		Task_FacialEmotionTask_OpenMouth(giant, 0.6f, "SneakVoreOpenMouth");

		for (auto& tiny: VoreData.GetVories()) {
			VoreController::GetSingleton().ShrinkOverTime(giant, tiny);
		}
    }
    void GTS_Sneak_Vore_Swallow(AnimationEventData& data) {
        Actor* giant = &data.giant;

        auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);

		if (!AllowDevourment()) {
			Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundSwallow, giant, 1.0f, "NPC Head [Head]"); // Play sound
		}

		for (auto& tiny: VoreData.GetVories()) {
			AllowToBeCrushed(tiny, true);
			if (tiny->formID == 0x14) {
				PlayerCamera::GetSingleton()->cameraTarget = giant->CreateRefHandle();
			}
			if (AllowDevourment()) {
				CallDevourment(giant, tiny);
				SetBeingHeld(tiny, false);
				VoreData.AllowToBeVored(true);
			} else {
				VoreData.Swallow();
				tiny->SetAlpha(0.0f);

				auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);
				if (tiny) {
					AllowToBeCrushed(tiny, true);
					EnableCollisions(tiny);
				}
				VoreData.AllowToBeVored(true);
				VoreData.KillAll();
				VoreData.ReleaseAll();
			}
		}

		ManageCamera(giant, false, CameraTracking::ObjectA);
		ManageCamera(giant, false, CameraTracking::Hand_Right);
    }

    void GTS_Sneak_Vore_CloseMouth(AnimationEventData& data) {
    }

    void GTS_Sneak_Vore_KillAll(AnimationEventData& data) {
    }

    void GTS_Sneak_Vore_SmileOn(AnimationEventData& data) {
        AdjustFacialExpression(&data.giant, 2, 1.0f, CharEmotionType::Expression, 0.32f, 0.72f);
		AdjustFacialExpression(&data.giant, 3, 0.8f, CharEmotionType::Phenome, 0.32f, 0.72f);
    }
    void GTS_Sneak_Vore_SmileOff(AnimationEventData& data) {
        AdjustFacialExpression(&data.giant, 2, 0.0f, CharEmotionType::Expression, 0.32f, 0.72f);
		AdjustFacialExpression(&data.giant, 3, 0.0f, CharEmotionType::Phenome, 0.32f, 0.72f);
    }
}

namespace GTS
{
	void Animation_VoreSneak::RegisterEvents() { 
		AnimationManager::RegisterEvent("GTS_Sneak_Vore_Start", "SneakVore", GTS_Sneak_Vore_Start);
		AnimationManager::RegisterEvent("GTS_Sneak_Vore_Grab", "SneakVore", GTS_Sneak_Vore_Grab);
        AnimationManager::RegisterEvent("GTS_Sneak_Vore_OpenMouth", "SneakVore", GTS_Sneak_Vore_OpenMouth);
        AnimationManager::RegisterEvent("GTS_Sneak_Vore_Swallow", "SneakVore", GTS_Sneak_Vore_Swallow);
        AnimationManager::RegisterEvent("GTS_Sneak_Vore_CloseMouth", "SneakVore", GTS_Sneak_Vore_CloseMouth);
        AnimationManager::RegisterEvent("GTS_Sneak_Vore_KillAll", "SneakVore", GTS_Sneak_Vore_KillAll);
        AnimationManager::RegisterEvent("GTS_Sneak_Vore_SmileOn", "SneakVore", GTS_Sneak_Vore_SmileOn);
        AnimationManager::RegisterEvent("GTS_Sneak_Vore_SmileOff", "SneakVore", GTS_Sneak_Vore_SmileOff);
	}
}