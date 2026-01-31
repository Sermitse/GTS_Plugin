#include "Managers/Animation/Grab_Sneak_Vore.hpp"

#include "Managers/Animation/Controllers/VoreController.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Grab.hpp"

using namespace GTS;

namespace {
    void GTS_GrabSneak_Start(AnimationEventData& data) { // Register Tiny for Vore
		auto otherActor = Grab::GetHeldActor(&data.giant);

        ManageCamera(&data.giant, true, CameraTracking::Grab_Left);
		auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);

		if (otherActor) {
			VoreData.AddTiny(otherActor);
		}
	}

    void GTS_GrabSneak_Eat(AnimationEventData& data) { 
		auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);
		for (auto& tiny: VoreData.GetVories()) {
            if (tiny) {
                tiny->NotifyAnimationGraph("JumpFall");
				tiny->Attacked(&data.giant);
            }
			VoreData.GrabAll(); // Switch to AnimObjectA attachment
		}
	}

	void GTS_GrabSneak_CamOff(AnimationEventData& data) {
		ManageCamera(&data.giant, false, CameraTracking::ObjectA);
		ManageCamera(&data.giant, false, CameraTracking::Grab_Left);
	}

    void GTS_GrabSneak_KillAll(AnimationEventData& data) {
		auto& VoreData = VoreController::GetSingleton().GetVoreData(&data.giant);
		for (auto& tiny: VoreData.GetVories()) {
			if (tiny) {
				AllowToBeCrushed(tiny, true);
				EnableCollisions(tiny);
			}
		}
		VoreData.AllowToBeVored(true);
		VoreData.KillAll();
		VoreData.ReleaseAll();
    }
    // Rest is handled inside Vore_Sneak (some events are re-used)
}

namespace GTS {
    void Animation_GrabSneak_Vore::RegisterEvents() { 
		AnimationManager::RegisterEvent("GTS_GrabSneak_Start", "SneakVore", GTS_GrabSneak_Start);
        AnimationManager::RegisterEvent("GTS_GrabSneak_Eat", "SneakVore", GTS_GrabSneak_Eat);
		AnimationManager::RegisterEvent("GTS_GrabSneak_CamOff", "SneakVore", GTS_GrabSneak_CamOff);
        AnimationManager::RegisterEvent("GTS_GrabSneak_KillAll", "SneakVore", GTS_GrabSneak_KillAll);
    }
}