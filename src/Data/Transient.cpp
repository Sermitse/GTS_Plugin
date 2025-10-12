#include "Data/Transient.hpp"

namespace GTS {

	TempActorData* Transient::GetData(TESObjectREFR* a_Object) {

		std::unique_lock lock(this->_Lock);

		if (!a_Object) {
			return nullptr;
		}

		auto ActorKey = a_Object->formID;
		TempActorData* result;

		try {

			if (!this->TempActorDataMap.contains(ActorKey)) {
				return nullptr;
			}

			result = &this->TempActorDataMap.at(ActorKey);
		}
		catch (const std::exception&) {
			return nullptr;
		}

		return result;
	}

	TempActorData* Transient::GetActorData(Actor* actor) {
		std::unique_lock lock(this->_Lock);

		if (!actor) {
			return nullptr;
		}
		auto ActorKey = actor->formID;

		auto tryAdd = [&]() -> TempActorData* {
			// (Re)check any conditions before adding.
			if (get_scale(actor) < 0.0f) {
				return nullptr;
			}
			// Create and emplace the new data.
			auto [iter, inserted] = this->TempActorDataMap.try_emplace(ActorKey, TempActorData(actor));
			return &(iter->second);
		};

		try {
			if (!this->TempActorDataMap.contains(ActorKey)) {
				return tryAdd();
			}
			return &this->TempActorDataMap.at(ActorKey);
		}
		catch (const std::out_of_range&) {
			// If out_of_range is thrown, try to add the data.
			return tryAdd();
		}
		catch (const std::exception& e) {
			logger::warn("Transient Exception GetActorData {}", e.what());
			return nullptr;
		}
	}

	std::vector<FormID> Transient::GetForms() const {
		std::unique_lock lock(this->_Lock);
		std::vector<FormID> keys;
		keys.reserve(this->TempActorDataMap.size());
		for(const auto data : this->TempActorDataMap | views::keys) {
			keys.push_back(data);
		}
		return keys;
	}


	std::string Transient::DebugName() {
		return "::Transient";
	}

	void Transient::ActorLoaded(RE::Actor* actor) {
		std::unique_lock lock(this->_Lock);
		if (!actor) {
			return;
		}
		const FormID ActorID = actor->formID;

		auto tryAdd = [&] {
			if (get_scale(actor) < 0.0f) {
				return;
			}
			this->TempActorDataMap.try_emplace(ActorID, TempActorData(actor));
		};

		try {
			// If the actor data is not already in the map, try to add it.
			if (!this->TempActorDataMap.contains(ActorID)) {
				tryAdd();
			}
		}
		catch (const std::out_of_range&) {
			// If an out_of_range exception occurs, try adding again.
			tryAdd();
		}
		catch (const std::exception& e) {
			logger::warn("Transient Exception ActorLoaded {}", e.what());
		}
	}

	void Transient::Reset() {
		std::unique_lock lock(this->_Lock);
		this->TempActorDataMap.clear();
		log::info("Transient was reset");
	}

	void Transient::ResetActor(Actor* actor) {
		std::unique_lock lock(this->_Lock);
		if (actor) {
			auto key = actor->formID;
			this->TempActorDataMap.erase(key);
		}
	}

	void Transient::EraseUnloadedTransientData() {
		std::unique_lock lock(this->_Lock);

		// Create a set to hold the whitelisted FormIDs.
		std::unordered_set<FormID> allowedFormIDs;

		// Always keep FormID 0x14 (Player).
		allowedFormIDs.insert(0x14);

		// Get preserve all currently loaded actors
		for (const Actor* ActorToNotDelete : find_actors()) {
			if (ActorToNotDelete) {
				allowedFormIDs.insert(ActorToNotDelete->formID);
			}
		}

		// Iterate through ActorDataMap and remove entries whose key is not in allowedFormIDs.
		for (auto it = TempActorDataMap.begin(); it != TempActorDataMap.end(); ) {
			if (!allowedFormIDs.contains(it->first)) {
				it = TempActorDataMap.erase(it);  // erase returns the next iterator.
			}
			else {
				++it;
			}
		}
		logger::critical("All Unloaded actors have beeen purged from transient.");
	}
}
