#include "Hooks/Engine/Input.hpp"
#include "Managers/Input/InputManager.hpp"
#include "Hooks/Util/HookUtil.hpp"

#include "UI/GTSMenu.hpp"

using namespace GTS;

namespace Hooks {

	struct InputDispatchEvent {

		static void thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent** a_events) {

			if (!a_events) {
				func(a_dispatcher, a_events);
				return;
			}

			{
				GTS_PROFILE_ENTRYPOINT("EngineInput::InputDispatchEvent");
				GTSMenu::GetSingleton().ProcessAndFilterEvents(a_events);      //UI, If an input consumer is visible, all input events except for mouse move are erased
				InputManager::GetSingleton().ProcessAndFilterEvents(a_events);   //GTS Inputs, Keys matching a triggered GTS action are removed.
			}

			func(a_dispatcher, a_events);
		}

		FUNCTYPE_CALL func;

	};

	void Hook_Input::Install() {

		logger::info("Installing Input Hooks...");

		stl::write_call<InputDispatchEvent>(REL::RelocationID(67315, 68617, NULL), REL::VariantOffset(0x7B, 0x7B, NULL));

	}

}
