#include "Hooks/Engine/Main.hpp"
#include "Hooks/Util/HookUtil.hpp"

namespace Hooks {

	struct MainUpdatePost {

		static inline std::atomic_bool started = false;

		static void thunk(RE::Main* a_this, float a_deltaTime) {

			func(a_this, a_deltaTime);

			{
				GTS_PROFILE_ENTRYPOINT("EngineMain::MainUpdatePost");

				State::SetOnMainThread(true);

				if (State::Live()) {

					// We are not loading or in the mainmenu
					// Player loaded and not paused
					if (started.exchange(true)) {
						// Not first updated
						Time::Update();
						EventDispatcher::DoUpdate();
					}
					else {
						// First update this load
						EventDispatcher::DoStart();
					}
				}
				else if (!State::InGame()) {
					// Loading or in main menu
					started.store(false);
				}
				State::SetOnMainThread(false);
			}

		}

		FUNCTYPE_CALL func;
	};


	void Hook_MainUpdate::Install() {
		logger::info("Installing Main Update Hooks...");
		//Update happens at the end of the Main::Update loop right before a BSLightingShader subroutine.
		stl::write_call<MainUpdatePost>(REL::RelocationID(35565, 36564, NULL), REL::VariantOffset(0x748, 0xC26, NULL));
	}

}

