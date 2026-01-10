#include "Managers/Animation/Grab_Vore.hpp"

#include "Managers/Animation/Controllers/VoreController.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Grab.hpp"

using namespace GTS;

namespace {

    void GTSGrab_Eat_Start(AnimationEventData& data) {
		auto otherActor = Grab::GetHeldActor(&data.giant);
		auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);
		ManageCamera(&data.giant, true, CameraTracking::Grab_Left);
		if (otherActor) {
			VoreData.AddTiny(otherActor);
		}
		StartLHandRumble("GrabVoreL", data.giant, 0.5f, 0.10f);
	}

	void GTSGrab_Eat_OpenMouth(AnimationEventData& data) {
		auto giant = &data.giant;
		auto otherActor = Grab::GetHeldActor(giant);
		auto& VoreData = VoreController::GetSingleton().GetVoreData(giant);
		if (otherActor) {
			SetBeingEaten(otherActor, true);
			VoreController::GetSingleton().ShrinkOverTime(giant, otherActor);
		}
		Task_FacialEmotionTask_OpenMouth(giant, 1.1f, "GrabVoreOpenMouth", 0.3f);
		StopLHandRumble("GrabVoreL", data.giant);
	}

	void GTSGrab_Eat_Eat(AnimationEventData& data) {
		auto otherActor = Grab::GetHeldActor(&data.giant);
		auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);
		if (otherActor) {
			for (auto& tiny: VoreData.GetVories()) {
				if (!AllowDevourment()) {
					VoreData.Swallow();
					if (AnimationVars::Crawl::IsCrawling(&data.giant)) {
						otherActor->SetAlpha(0.0f); // Hide Actor
					}
				} else {
					CallDevourment(&data.giant, otherActor);
				}
			}
		}
	}

	void GTSGrab_Eat_CloseMouth(AnimationEventData& data) {
	}

	void GTSGrab_Eat_Swallow(AnimationEventData& data) {
		auto giant = &data.giant;
		auto otherActor = Grab::GetHeldActor(&data.giant);
		if (otherActor) {
			SetBeingEaten(otherActor, false);
			auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);
			for (auto& tiny: VoreData.GetVories()) {
				VoreData.KillAll();
			}
			AnimationVars::Grab::SetHasGrabbedTiny(giant, false);
			AnimationVars::Grab::SetGrabState(giant, false);
			Runtime::PlaySoundAtNode(Runtime::SNDR.GTSSoundSwallow, &data.giant, 1.0f, "NPC Head [Head]"); // Play sound
			AnimationManager::StartAnim("TinyDied", giant);
			//BlockFirstPerson(giant, false);
			ManageCamera(&data.giant, false, CameraTracking::Grab_Left);
			SetBeingHeld(otherActor, false);
			Grab::DetachActorTask(giant);
			Grab::Release(giant);
		}
	}
}

namespace GTS {
    void Animation_GrabVore::RegisterEvents() {
        AnimationManager::RegisterEvent("GTSGrab_Eat_Start", "Grabbing", GTSGrab_Eat_Start);
		AnimationManager::RegisterEvent("GTSGrab_Eat_OpenMouth", "Grabbing", GTSGrab_Eat_OpenMouth);
		AnimationManager::RegisterEvent("GTSGrab_Eat_Eat", "Grabbing", GTSGrab_Eat_Eat);
		AnimationManager::RegisterEvent("GTSGrab_Eat_CloseMouth", "Grabbing", GTSGrab_Eat_CloseMouth);
		AnimationManager::RegisterEvent("GTSGrab_Eat_Swallow", "Grabbing", GTSGrab_Eat_Swallow);
    }
}