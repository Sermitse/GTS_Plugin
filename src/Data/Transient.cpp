#include "Data/Transient.hpp"

namespace GTS {

	std::string Transient::DebugName() {
		return "::Transient";
	}

	void Transient::ActorLoaded(RE::Actor* actor) {
		std::unique_lock lock(_Lock);
		if (!actor) {
			return;
		}
		const FormID ActorID = actor->formID;

		auto tryAdd = [&] {
			if (get_scale(actor) < 0.0f) {
				return;
			}
			TempActorDataMap.try_emplace(ActorID, TransientActorData(actor));
		};

		try {
			// If the actor data is not already in the map, try to add it.
			if (!TempActorDataMap.contains(ActorID)) {
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
		std::unique_lock lock(_Lock);
		TempActorDataMap.clear();
		logger::info("Transient was reset");
	}

	void Transient::ResetActor(Actor* actor) {
		std::unique_lock lock(_Lock);
		if (actor) {
			auto key = actor->formID;
			TempActorDataMap.erase(key);
		}
	}

	void Transient::OnGameRevert() {
		Reset();
	}

	TransientActorData* Transient::GetActorData(Actor* actor) {
		std::unique_lock lock(_Lock);

		if (!actor) {
			return nullptr;
		}
		auto ActorKey = actor->formID;

		auto tryAdd = [&]() -> TransientActorData* {
			// (Re)check any conditions before adding.
			if (get_scale(actor) < 0.0f) {
				return nullptr;
			}
			// Create and emplace the new data.
			auto [iter, inserted] = TempActorDataMap.try_emplace(ActorKey, TransientActorData(actor));
			return &(iter->second);
		};

		try {
			if (!TempActorDataMap.contains(ActorKey)) {
				return tryAdd();
			}
			return &TempActorDataMap.at(ActorKey);
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

	void Transient::EraseUnloadedData() {
		std::unique_lock lock(_Lock);

		// Create a set to hold the whitelisted FormIDs.
		absl::flat_hash_set<FormID> allowedFormIDs;

		// Always keep FormID 0x14 (Player).
		allowedFormIDs.insert(0x14);

		// Get preserve all currently loaded actors
		for (const Actor* ActorToNotDelete : find_actors()) {
			if (ActorToNotDelete) {
				allowedFormIDs.insert(ActorToNotDelete->formID);
			}
		}

		absl::erase_if(TempActorDataMap, [&](const auto& entry) {
			return !allowedFormIDs.contains(entry.first);
		});

		logger::critical("All Unloaded actors have beeen purged from transient.");
	}
}
