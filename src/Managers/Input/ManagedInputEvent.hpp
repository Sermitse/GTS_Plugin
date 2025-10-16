#pragma once

#include "Config/Settings/SettingsKeybinds.hpp"

namespace GTS {

	enum class LInputEventState_t : std::uint8_t {
		Idle,
		Held,
	};

	class ManagedInputEvent {
		public:

		explicit ManagedInputEvent(const BaseEventData_t& a_event);

		// Return time since it was first pressed
		[[nodiscard]] float Duration() const;

		// Will take a key list and process if the event should fire.
		// will return true if the events conditions are met
		[[nodiscard]] bool ShouldFire(const std::unordered_set<std::uint32_t>& keys);

		// Returns true if all keys are pressed this frame
		// Not taking into account things like duration
		[[nodiscard]] bool AllKeysPressed(const std::unordered_set<std::uint32_t>& keys) const;

		// Returns true if ONLY the specicified keys are pressed this frame
		// Not taking into account things like duration
		[[nodiscard]] bool OnlyKeysPressed(const std::unordered_set<std::uint32_t>& keys) const;

		// Resets the timer and all appropiate state variables
		void Reset();

		// Returns the duration required for the event to fire
		[[nodiscard]] float MinDuration() const;

		// Returns if the event is a onup event
		[[nodiscard]] bool IsOnUp() const;

		[[nodiscard]] std::string GetName() const;

		// Check if this is an On key up event
		//bool IsOnUp();
		[[nodiscard]] bool HasKeys() const;

		// Checks if this key is the same as another in terms
		// of mutaally exclusive triggers
		[[nodiscard]] bool SameGroup(const ManagedInputEvent& other) const;

		[[nodiscard]] unordered_set<std::uint32_t> GetKeys();

		[[nodiscard]] LBlockInputTypes_t ShouldBlock() const;

		[[nodiscard]] bool IsDisabled() const;

		private:

		double startTime = 0.0;
		bool primed = false; // Used for release events. Once primed, when keys are not pressed we fire

		std::string name;
		unordered_set<std::uint32_t> keys = {};
		float minDuration = 0.0f;

		// If true this event won't fire unless ONLY the keys are pressed for the entire duration
		bool exclusive = false;
		bool Disabled = false;
		LTriggerType_t trigger = LTriggerType_t::Once;
		LInputEventState_t state = LInputEventState_t::Idle;
		LBlockInputTypes_t blockinput = LBlockInputTypes_t::Automatic;
	};
}