#pragma once

#include "Managers/Input/ManagedInputEvent.hpp"

namespace GTS {

	struct RegisteredInputEvent {
		std::function<void(const ManagedInputEvent&)> callback = nullptr;
		std::function<bool(void)> condition = nullptr;

		RegisteredInputEvent(const std::function<void(const ManagedInputEvent&)>
			& callback, const std::function<bool(void)>& condition) : 
			callback(callback) , condition(condition){
		}
	};

	using EventResult = RE::BSEventNotifyControl;

	class InputManager : EventListener {
		public:
			[[nodiscard]] static InputManager& GetSingleton() noexcept;

			void ProcessAndFilterEvents(InputEvent** a_event);

			std::string DebugName() override;

			static void Init();

			static void RegisterInputEvent(std::string_view namesv, std::function<void(const ManagedInputEvent&)> 
				callback, std::function<bool(void)> condition = nullptr);

		private:

		static std::vector<ManagedInputEvent> LoadInputEvents();

		std::atomic_bool Ready = false;
		std::mutex LoadLock;
		std::unordered_map<std::string, RegisteredInputEvent> registedInputEvents;
		std::vector<ManagedInputEvent> ManagedTriggers;
	};
}
