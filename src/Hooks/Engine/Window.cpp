#include "Hooks/Engine/Window.hpp"
#include "Hooks/Util/HookUtil.hpp"
#include "UI/GTSMenu.hpp"

namespace Hooks {

	//Win32 Window Message Handler
	struct WndProcHandler {

		static LRESULT thunk(HWND a_hwnd, UINT a_msg, WPARAM a_wParam, LPARAM a_lParam) {

			{
				GTS_PROFILE_ENTRYPOINT("EnginePresent::WndProcHandler");

				//On focus loss/gain
				if (a_msg == WM_ACTIVATEAPP) {

					if (GTSMenu::Ready()) {
						GTSMenu::ImIO->ClearEventsQueue();
						GTSMenu::ImIO->ClearInputMouse();
						GTSMenu::ImIO->ClearInputKeys();
					}
				}
			}

			return func(a_hwnd, a_msg, a_wParam, a_lParam);
		}

		FUNCTYPE_CALL func;

	};

	//Win32 RegisterClassA
	struct RegisterClassA {

		static uint16_t thunk(WNDCLASSA* a_wndClass) {

			{

				GTS_PROFILE_ENTRYPOINT("EnginePresent::RegisterClassA");

				WndProcHandler::func = reinterpret_cast<uintptr_t>(a_wndClass->lpfnWndProc);
				a_wndClass->lpfnWndProc = &WndProcHandler::thunk;
			}

			return func(a_wndClass);
		}

		FUNCTYPE_CALL func;

	};

	void Hook_Window::Install() {
		logger::info("Installing Present Hooks...");
		stl::write_call<RegisterClassA, 6>(REL::RelocationID(75591, 77226, NULL), REL::VariantOffset(0x8E, 0x15C, NULL));
	}

}
