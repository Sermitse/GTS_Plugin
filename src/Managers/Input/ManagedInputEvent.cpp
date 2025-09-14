#include "Managers/Input/ManagedInputEvent.hpp"

namespace GTS {

	//Construct the Managed Input Event...

	//TODO Patched together from old code, should really be rewritten entirely...

	ManagedInputEvent::ManagedInputEvent(const BaseEventData_t& a_event) {

		this->Disabled = a_event.Disabled;
		this->name = a_event.Event;
		float duration = a_event.Duration;
		this->exclusive = a_event.Exclusive;
		this->trigger = StringToEnum<LTriggerType_t>(a_event.Trigger);
		this->blockinput = StringToEnum<LBlockInputTypes_t>(a_event.BlockInput);
		this->minDuration = duration;
		this->startTime = 0.0;
		this->keys = {};

		const auto& EventKeys = a_event.Keys;

		for (const auto& key : EventKeys) {
			std::string upper_key = str_toupper(remove_whitespace(key));
			if (upper_key != "LEFT" && upper_key != "DIK_LEFT") {
				// This changes LEFTALT to LALT
				// But NOT LEFT into L
				replace_first(upper_key, "LEFT", "L");
			}
			if (upper_key != "RIGHT" && upper_key != "DIK_RIGHT") {
				replace_first(upper_key, "RIGHT", "R");
			}
			try {
				std::uint32_t key_code = NAMED_KEYS.at(upper_key);
				this->keys.emplace(key_code);
			}
			catch (const std::out_of_range&) {
				log::warn("Key named {}=>{} in {} was unrecongized.", key, upper_key, this->name);
				this->keys.clear();
				return; // Remove all keys and return so that this becomes an INVALID key entry and won't fire
			}
		}
	}

	bool ManagedInputEvent::IsDisabled() const {
		return this->Disabled;
	}

	float ManagedInputEvent::Duration() const {
		return static_cast<float>(Time::WorldTimeElapsed() - this->startTime);
	}

	float ManagedInputEvent::MinDuration() const {
		return this->minDuration;
	}

	void ManagedInputEvent::Reset() {
		this->startTime = Time::WorldTimeElapsed();
		this->state = LInputEventState_t::Idle;
		this->primed = false;
	}

	bool ManagedInputEvent::IsOnUp() const {
		return this->trigger == LTriggerType_t::Release;
	}

	bool ManagedInputEvent::SameGroup(const ManagedInputEvent& other) const {
		if (this->IsOnUp() && other.IsOnUp()) {
			return this->keys == other.keys;
		}
		return false;
	}

	bool ManagedInputEvent::AllKeysPressed(const std::unordered_set<std::uint32_t>& keys) const {

		if (this->keys.empty()) {
			return false;
		}

		for (const auto& key : this->keys) {
			if (!keys.contains(key)) {
				// Key not found
				return false;
			}
		}
		return true;
	}

	bool ManagedInputEvent::OnlyKeysPressed(const std::unordered_set<std::uint32_t>& keys_in) const {
		std::unordered_set<std::uint32_t> keys(keys_in); // Copy
		for (const auto& key : this->keys) {
			keys.erase(key);
		}
		return keys.empty();
	}

	bool ManagedInputEvent::ShouldFire(const std::unordered_set<std::uint32_t>& a_gameInputKeys) {
		bool shouldFire = false;
		// Check based on keys and duration
		if (this->AllKeysPressed(a_gameInputKeys) && (!this->exclusive || this->OnlyKeysPressed(a_gameInputKeys))) {
			shouldFire = true;
		}
		else {
			// Keys aren't held reset the start time of the button hold
			this->startTime = Time::WorldTimeElapsed();
			// and reset the state to idle
			this->state = LInputEventState_t::Idle;
		}
		// Check based on duration
		if (shouldFire) {
			if (this->minDuration > 0.0) {
				// Turn it off if duration is not met
				shouldFire = this->Duration() > this->minDuration;
			}
		}

		// Check based on held and trigger state
		if (shouldFire) {
			this->primed = true;
			switch (this->state) {
				case LInputEventState_t::Idle: {
					this->state = LInputEventState_t::Held;
					switch (this->trigger) {
						// If once or continius start firing now
	
						case LTriggerType_t::Once:
						case LTriggerType_t::Continuous: {
							return true;
						}
						case LTriggerType_t::Release: {
							return false;
						}
						default:{
							log::warn("Unexpected TriggerMode.");
							return false; // Catch if something goes weird
						}
					}
				}
				case LInputEventState_t::Held: {
					switch (this->trigger) {
						// If once stop firing

						case LTriggerType_t::Once:
						case LTriggerType_t::Release:{
							// For release still do nothing
							return false;
						}
						case LTriggerType_t::Continuous:{
							// if continous keep firing
							return true;
						}

						default:{
							log::error("Unexpected TriggerMode.");
							return false; // Catch if something goes weird
						}
					}
				}
				default:{
					log::error("Unexpected InputEventState.");
					return false; // Catch if something goes weird
				}
			}
		}
		
		if (this->primed) {
			this->primed = false;
			switch (this->trigger) {
				case LTriggerType_t::Release:{
					// For release fire now that we have stopped pressing
					return true;
				}
				default:{
					return false;
				}
			}
		}
		return false;
	}

	bool ManagedInputEvent::HasKeys() const {
		return !this->keys.empty();
	}

	std::string ManagedInputEvent::GetName() const {
		return this->name;
	}

	unordered_set<std::uint32_t> ManagedInputEvent::GetKeys() {
		return keys;
	}

	LBlockInputTypes_t ManagedInputEvent::ShouldBlock() const {
		return this->blockinput;
	}
}