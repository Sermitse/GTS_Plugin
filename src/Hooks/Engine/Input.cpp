#include "Hooks/Engine/Input.hpp"
#include "Managers/Input/InputManager.hpp"
#include "UI/UIManager.hpp"
#include "Hooks/Util/HookUtil.hpp"

using namespace GTS;

namespace Hooks {

	struct InputDispatchEvent {

		static void thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent** a_events) {

			GTS_PROFILE_ENTRYPOINT("EngineInput::InputDispatchEvent");

			if (!a_events) {
				func(a_dispatcher, a_events);
				return;
			}

			if (UIManager::GetSingleton().InputUpdate(a_events)) {
				constexpr RE::InputEvent* const dummy[] = { nullptr };
				func(a_dispatcher, const_cast<RE::InputEvent**>(dummy));
				return;
			}

			InputManager::GetSingleton().ProcessEvents(a_events);

			func(a_dispatcher, a_events);
		}

		FUNCTYPE_CALL func;

	};

	void Hook_Input::Install() {

		logger::info("Installing Input Hooks...");

		stl::write_call<InputDispatchEvent>(REL::RelocationID(67315, 68617, NULL), REL::VariantOffset(0x7B, 0x7B, NULL));

	}

}
