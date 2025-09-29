#include "Hooks/Engine/Present.hpp"
#include "UI/UIManager.hpp"
#include "Hooks/Util/HookUtil.hpp"

namespace Hooks {

	//Win32 Window Message Handler
	struct WndProcHandler {

		static LRESULT thunk(HWND a_hwnd, UINT a_msg, WPARAM a_wParam, LPARAM a_lParam) {

			{
				GTS_PROFILE_ENTRYPOINT("EnginePresent::WndProcHandler");

				//On focus loss/gain
				if (a_msg == WM_ACTIVATEAPP) {

					if (UIManager::Ready()) {

						//IO can only be retrieved if a valid imgui context exists.
						auto& io = ImGui::GetIO();
						io.ClearEventsQueue();
						io.ClearInputMouse();
						io.ClearInputKeys();

					}
				}

			}

			return func(a_hwnd, a_msg, a_wParam, a_lParam);
		}

		FUNCTYPE_CALL func;

	};

	//DXGI Swapchain Create
	struct CreateD3DAndSwapChain {

		static void thunk() {

			func();

			{
				GTS_PROFILE_ENTRYPOINT("EnginePresent::CreateD3DAndSwapChain");
				UIManager::GetSingleton().Init();
			}

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

	//DXGISwapchain Present
	struct DXGISwapchainPresent {

		static void thunk(uint32_t unk_01) {

			func(unk_01);

			{
				GTS_PROFILE_ENTRYPOINT("EnginePresent::DXGISwapchainPresent");
				UIManager::GetSingleton().Present(ImWindow::DrawLevel::kPresent);
			}
		}

		FUNCTYPE_CALL func;

	};




	void Hook_Present::Install() {

		logger::info("Installing Present Hooks...");

		stl::write_call<RegisterClassA, 6>(REL::RelocationID(75591, 77226, NULL), REL::VariantOffset(0x8E, 0x15C, NULL));
		stl::write_call<CreateD3DAndSwapChain>(REL::RelocationID(75595, 77226, NULL), REL::VariantOffset(0x9, 0x275, NULL));
		stl::write_call<DXGISwapchainPresent>(REL::RelocationID(75461, 77246, NULL), REL::VariantOffset(0x9, 0x9, NULL));

	}

}
