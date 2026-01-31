#include "Hooks/Sound/BGSImpactManager.hpp"
#include "Hooks/Util/HookUtil.hpp"
#include "Managers/Impact.hpp"

using namespace GTS;

namespace Hooks {

	struct PlayImpactDataSounds {

		static bool thunk(BGSImpactManager* a_manager, BGSImpactManager::ImpactSoundData& a_data) {


			{
				GTS_PROFILE_ENTRYPOINT("SoundImpactManager::PlayImpactDataSounds");

				// Trying to get/alter data.sound1/sound2 is useless since they never pass the if (sound) check
				// So the only way to disable default sounds is to do data.playsound1/2 = false

				if (auto& Object = a_data.objectToFollow) {
					// GetUserData seems to be player exclusive for some reason
					if (auto userData = Object->GetUserData()) {
						Actor* actor = skyrim_cast<Actor*>(userData);
						if (actor) {
							if (actor->IsPlayerRef()) {
								float scale = get_visual_scale(actor);
								if (HasSMT(actor)) {
									scale *= 2.0f;
								}
								if (scale > 1.75f) {
									//log::info("Disabling sounds for {}", actor->GetDisplayFullName());
									a_data.playSound1 = false;
									a_data.playSound2 = false;
								}
							}
						}
					}
				}
			}

			return func(a_manager, a_data);
		}

		FUNCTYPE_DETOUR func;
	};


	struct BGSImpactManagerProcessEvent {

		static constexpr size_t funcIndex = 0x01;

		static BSEventNotifyControl thunk(BGSImpactManager* a_this, const BGSFootstepEvent* a_event, BSTEventSource<BGSFootstepEvent>* a_eventSource) {

			{
				GTS_PROFILE_ENTRYPOINT("SoundImpactManager::ImpactManagerProcessEvent");
				ImpactManager::HookProcessEvent(a_this, a_event, a_eventSource);
				// ^ On FootEvent: manages damage, effects and launching. do NOT disable it!

			}

			return func(a_this, a_event, a_eventSource);
		}

		FUNCTYPE_VFUNC func;

	};


	void Hook_BGSImpactManager::Install() {

		logger::info("Installing BGSImpactManager Hooks...");

		stl::write_detour<PlayImpactDataSounds>(REL::RelocationID(35317, 36212, NULL));
		stl::write_vfunc<BGSImpactManagerProcessEvent>(VTABLE_BGSImpactManager[0]);

	}

}
