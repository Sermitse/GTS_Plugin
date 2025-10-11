#pragma once

#include "Managers/Input/ManagedInputEvent.hpp"

namespace GTS {

	using EventResult = RE::BSEventNotifyControl;

	struct RegisteredInputEvent {
		std::function<void(const ManagedInputEvent&)> callback = nullptr;
		std::function<bool(void)> condition = nullptr;

		RegisteredInputEvent(const std::function<void(const ManagedInputEvent&)>& a_funcCallback, const std::function<bool(void)>& a_condCallback) : 
			callback(a_funcCallback) , condition(a_condCallback){
		}
	};

	class InputManager : EventListener, public CInitSingleton<InputManager> {
		public:
		void ProcessAndFilterEvents(InputEvent** a_event);
		std::string DebugName() override;
		void Init();
		static void RegisterInputEvent(std::string_view a_namesv, std::function<void(const ManagedInputEvent&)> a_funcCallback, std::function<bool(void)> a_condCallbakc = nullptr);

		private:
		static std::vector<ManagedInputEvent> LoadInputEvents();
		std::atomic_bool m_ready = false;
		std::mutex m_lock;
		std::unordered_map<std::string, RegisteredInputEvent> m_inputEvents;
		std::vector<ManagedInputEvent> m_eventTriggers;
	};
}
