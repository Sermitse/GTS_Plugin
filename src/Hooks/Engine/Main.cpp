#include "Hooks/Engine/Main.hpp"
#include "Data/Time.hpp"
#include "Data/Plugin.hpp"
#include "Hooks/Util/HookUtil.hpp"

namespace Hooks {

	struct MainUpdate {

		static void thunk(RE::Main* a_this, float a_deltaTime) {

			func(a_this, a_deltaTime);

			GTS_PROFILE_ENTRYPOINT("EngineMain::MainUpdate");

			static std::atomic_bool started = std::atomic_bool(false);
			Plugin::SetOnMainThread(true);

			if (Plugin::Live()) {

				//Cache all currently loaded Actors
				// We are not loading or in the mainmenu
				// Player loaded and not paused
				if (started.exchange(true)) {
					// Not first updated
					Time::GetSingleton().Update();
					EventDispatcher::DoUpdate();
				}
				else {
					// First update this load
					EventDispatcher::DoStart();
				}
			}
			else if (!Plugin::InGame()) {
				// Loading or in main menu
				started.store(false);
			}
			Plugin::SetOnMainThread(false);
		}

		FUNCTYPE_CALL func;
	};


	void Hook_MainUpdate::Install() {
		//Update happens at the end of the main game loop right before a BSLightingShader subroutine.
		stl::write_call<MainUpdate>(REL::RelocationID(35565, 36564, NULL), REL::VariantOffset(0x748, 0xC26, NULL));
	}

}

