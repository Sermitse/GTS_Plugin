#include "Managers/Input/InputManager.hpp"
#include "Config/Keybinds.hpp"

namespace GTS {

	std::vector<ManagedInputEvent> InputManager::LoadInputEvents() {

		std::vector<ManagedInputEvent> results;

		for (const auto& GTSInputEvent : Keybinds::GetSingleton().InputEvents) {

			ManagedInputEvent newData(GTSInputEvent);

			if (newData.HasKeys()) {
				results.push_back(newData);
			}

		}

		// Sort longest duration first
		ranges::sort(results,[](ManagedInputEvent const& a, ManagedInputEvent const& b) {
			return a.MinDuration() > b.MinDuration();
		});

		return results;
	}

	InputManager& InputManager::GetSingleton() noexcept {
		static InputManager instance;
		return instance;
	}

	void InputManager::RegisterInputEvent(std::string_view namesv, std::function<void(const ManagedInputEvent&)> callback, std::function<bool(void)> condition) {
		auto& me = InputManager::GetSingleton();
		std::string name(namesv);
		me.registedInputEvents.try_emplace(name, callback, condition);
		log::debug("Registered input event: {}", namesv);
	}

	void InputManager::Init() {

		Ready.store(false);

		try {
			ManagedTriggers = LoadInputEvents();
		} 
		catch (exception e) {
			log::error("Error Creating ManagedInputEvents: {}", e.what());
			return;
		} 

		log::info("Loaded {} key bindings", ManagedTriggers.size());
		
		Ready.store(true);
	}

	void InputManager::ProcessEvents(InputEvent** a_event) {

		std::unordered_set<uint32_t> KeysToBlock = {};
		std::unordered_set<std::uint32_t> gameInputKeys = {};
		RE::InputEvent* event = *a_event;
		RE::InputEvent* prev = nullptr;

		if (!a_event) {
			return;
		}

		if (Plugin::AnyMenuOpen() || !Plugin::Live() || !Plugin::Ready()) {
			return;
		}

		if (!Ready.load()) {
			return;
		}

		GTS_PROFILE_SCOPE("InputManager: ProcessEvents");

		//Get Current InputKeys
		for (auto eventIt = *a_event; eventIt; eventIt = eventIt->next) {
			//If the event is not a button, ignore.
			if (eventIt->GetEventType() != INPUT_EVENT_TYPE::kButton) {
				continue;
			}

			//If the event is not a ButtonEvent or it is one but the event is "empty", ignore.
			ButtonEvent* buttonEvent = eventIt->AsButtonEvent();
			if (!buttonEvent || (!buttonEvent->IsPressed() && !buttonEvent->IsUp())) {
				continue;
			}

			//If it is a ButtonEvent add it to to the list of pressed keys
			if (buttonEvent->device.get() == INPUT_DEVICE::kKeyboard) {
				auto key = buttonEvent->GetIDCode();
				gameInputKeys.emplace(key);
			}
			else if (buttonEvent->device.get() == INPUT_DEVICE::kMouse) {
				auto key = buttonEvent->GetIDCode();
				gameInputKeys.emplace(key + MOUSE_OFFSET);
			}
		}

		for (auto& trigger : this->ManagedTriggers) {

			if (trigger.IsDisabled()) continue;

			// Store triggers in here that have been fired this frame
			std::vector<ManagedInputEvent*> firedTriggers; 
			auto blockInput = trigger.ShouldBlock();

			//Are all keys pressed for this trigger and are we allowed to selectively block?
			//if never: behavior defaults to old implementation
			if (trigger.AllKeysPressed(gameInputKeys)){
				//log::debug("AllkeysPressed for trigger {}", trigger.GetName());
				//Get the coresponding event data
				try {
					auto& eventData = this->registedInputEvents.at(trigger.GetName());

					if (blockInput == BlockInputTypes::Always) {
						//If force blocking is set block game input regardless of conditions
						std::unordered_set<uint32_t> KeysToAdd = std::unordered_set<uint32_t>(trigger.GetKeys());
						KeysToBlock.insert(KeysToAdd.begin(), KeysToAdd.end());

						if (eventData.condition != nullptr) {
							if (!eventData.condition()) {
								continue;
							}
						}

					}
					//The condition callback can be null, check before calling it.
					//In the case it's null input blocking or early continuing won't be done and the system will behave like previously unless its forced.
					else if (eventData.condition != nullptr) {
						//log::debug("condition exists {}", fmt::ptr(&eventData.condition));
						//Used to verify wether this trigger will actually end up doing anthing
						if (eventData.condition()) {
							//log::debug("condition is true for {}", trigger.GetName());
							//Need to make a copy here otherwise insert throws an assertion

							if (blockInput != BlockInputTypes::Never) {
								std::unordered_set<uint32_t> KeysToAdd = std::unordered_set<uint32_t>(trigger.GetKeys());
								//log::debug("ShouldBlock is true for {}", trigger.GetName());
								KeysToBlock.insert(KeysToAdd.begin(), KeysToAdd.end());
							}
						}
						else {
							//log::debug("Condition Was False For Event: {}", trigger.GetName());
							//If False Skip calling ShouldFire as there is no point in processing an event that won't do anything
							continue;
						}
					}
				}

				catch (const std::out_of_range&) {
					log::warn("Event {} was triggered but there is no event of that name", trigger.GetName());
					continue;
				}
			}

			//Handles Event tiggering conditions
			if (trigger.ShouldFire(gameInputKeys)) {
				bool groupAlreadyFired = false;
				for (auto firedTrigger : firedTriggers) {
					if (trigger.SameGroup(*firedTrigger)) {
						groupAlreadyFired = true;
						break;
					}
				}

				if (groupAlreadyFired) {
					trigger.Reset();
				}
				else {
					log::debug("Running event {}", trigger.GetName());
					firedTriggers.push_back(&trigger);
					try {
						auto& eventData = this->registedInputEvents.at(trigger.GetName());
						eventData.callback(trigger);
					}
					catch (const std::out_of_range&) {
						log::warn("Event {} was triggered but there is no event of that name", trigger.GetName());
					}
				}
			}
		}

		while (event != nullptr) {
			bool shouldDispatch = true;
			if (event->eventType == RE::INPUT_EVENT_TYPE::kButton) {
				const auto button = skyrim_cast<RE::ButtonEvent*>(event);
				if (button) {
					uint32_t input = button->GetIDCode();
					if (KeysToBlock.contains(input)) {
						//logger::debug("Blocked Input For Key {}", input);
						shouldDispatch = false;
					}
				}
			}

			RE::InputEvent* nextEvent = event->next;
			if (!shouldDispatch) {
				if (prev != nullptr) {
					prev->next = nextEvent;
				}
				else {
					*a_event = nextEvent;
				}
			}
			else {
				prev = event;
			}
			event = nextEvent;
		}
	}

	std::string InputManager::DebugName() {
		return "::InputManager";
	}
}
