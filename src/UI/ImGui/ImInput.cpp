#include "UI/ImGui/ImInput.hpp"
#include "UI/UIManager.hpp"
#include "UI/ImGui/ImWindowManager.hpp"

namespace GTS {


	void ImInput::UnstickKeys() {

		ImGuiIO& io = ImGui::GetIO();

		if ((io.KeysData[ImGuiKey_LeftShift].Down && !(GetAsyncKeyState(VK_LSHIFT) & 0x8000)) || (io.KeysData[ImGuiKey_RightShift].Down && !(GetAsyncKeyState(VK_RSHIFT) & 0x8000))) {
			io.AddKeyEvent(ImGuiKey_LeftShift, false);
			io.AddKeyEvent(ImGuiKey_RightShift, false);
		}

		if (io.KeysData[ImGuiKey_Tab].Down && !(GetAsyncKeyState(VK_TAB) & 0x8000)) {
			io.AddKeyEvent(ImGuiKey_Tab, false);
		}
	}

	void ImInput::ProcessInputEventQueue() {

		std::unique_lock<std::shared_mutex> mutex(InputMutex);

		ImGuiIO& io = ImGui::GetIO();

		for (auto& event : KeyEventQueue) {

			if (event.eventType == RE::INPUT_EVENT_TYPE::kChar) {
				io.AddInputCharacter(event.keyCode);
				continue;
			}

			if (event.device == RE::INPUT_DEVICE::kMouse) {
				
				if (event.keyCode > 7) {
					io.AddMouseWheelEvent(0, event.value * (event.keyCode == 8 ? 1 : -1));
				}
				else {
					event.keyCode = std::min<uint32_t>(event.keyCode, 5);
					io.AddMouseButtonEvent(event.keyCode, event.IsPressed());
				}
			}

			if (event.device == RE::INPUT_DEVICE::kKeyboard) {

				uint32_t key = DIKToVK(event.keyCode);

				if (key == event.keyCode) {
					key = MapVirtualKeyEx(event.keyCode, MAPVK_VSC_TO_VK_EX, GetKeyboardLayout(0));
				}

				if (!event.IsPressed()) {

					if (auto MainWindow = ImWindowManager::GetSingleton().GetWindowByName("Settings")) {
						if (key == VK_ESCAPE && MainWindow->Show && !MainWindow->Busy) {
							UIManager::CloseSettings();
							continue;
						}
					}
				}

				io.AddKeyEvent(VirtualKeyToImGuiKey(key), event.IsPressed());

				if (key == VK_LCONTROL || key == VK_RCONTROL) {
					io.AddKeyEvent(ImGuiMod_Ctrl, event.IsPressed());
				}
				else if (key == VK_LSHIFT || key == VK_RSHIFT) {
					io.AddKeyEvent(ImGuiMod_Shift, event.IsPressed());
				}
				else if (key == VK_LMENU || key == VK_RMENU) {
					io.AddKeyEvent(ImGuiMod_Alt, event.IsPressed());
				}

				//Enable Advanced Settings
				if (io.KeyAlt && io.KeyShift && io.KeyCtrl && key == VK_F12) {

					if (auto MainWindow = ImWindowManager::GetSingleton().GetWindowByName("Settings")) {
						if (MainWindow->Show && !MainWindow->Busy) {
							Config::GetHidden().IKnowWhatImDoing = true;
						}
					}
				}
			}
		}

		KeyEventQueue.clear();
		UnstickKeys();
	}
}
