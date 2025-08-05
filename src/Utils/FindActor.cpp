#include "Utils/FindActor.hpp"

namespace GTS {
	/**
	 * Find actors in ai manager that are loaded
	 */

	class FindActorData {
		public:
		/// Actors that have been done recently;
		unordered_set<FormID> previousActors;
	};



	vector<Actor*> find_actors() {

		GTS_PROFILE_SCOPE("FindActor: FindActors");

		// Get the current thread ID
		std::thread::id current_thread = std::this_thread::get_id();
		std::thread::id stored_main_thread = main_update_thread_id.load();

		// If we're on the main update thread and cache is valid, return cached version
		if (current_thread == stored_main_thread && cache_valid.load()) {
			std::lock_guard<std::mutex> lock(cache_mutex);
			return cached_actors; // This will copy the vector
		}

		return find_actors_high();
	}

	vector<Actor*> find_actors_high() {

		const auto process_list = ProcessLists::GetSingleton();

		vector<Actor*> result;
		result.reserve(process_list->highActorHandles.size() + 1);

		for (const auto& actor_handle : process_list->highActorHandles) {
			if (!actor_handle) continue;

			auto actor_smartptr = actor_handle.get();
			if (!actor_smartptr) continue;

			Actor* actor = actor_smartptr.get();
			if (actor->Is3DLoaded()) {
				result.push_back(actor);
			}
		}

		auto* player = PlayerCharacter::GetSingleton();
		if (player && player->Is3DLoaded()) {
			result.push_back(player);
		}

		return result;
	}

	/*vector<Actor*> find_actors_middle_high() {
		vector<Actor*> result;

		auto process_list = ProcessLists::GetSingleton();
		for (const ActorHandle& actor_handle: process_list->middleHighActorHandles)
		{
			if (!actor_handle) {
				continue;
			}
			auto actor_smartptr = actor_handle.get();
			if (!actor_smartptr) {
				continue;
			}

			Actor* actor = actor_smartptr.get();
			// auto actor = actor_handle.get().get();
			if (actor && actor->Is3DLoaded()) {
				result.push_back(actor);
			}
		}
		return result;
	}

	vector<Actor*> find_actors_middle_low() {
		vector<Actor*> result;

		auto process_list = ProcessLists::GetSingleton();
		for (const ActorHandle& actor_handle: process_list->middleLowActorHandles)
		{
			if (!actor_handle) {
				continue;
			}
			auto actor_smartptr = actor_handle.get();
			if (!actor_smartptr) {
				continue;
			}

			Actor* actor = actor_smartptr.get();
			// auto actor = actor_handle.get().get();
			if (actor && actor->Is3DLoaded()) {
				result.push_back(actor);
			}
		}
		return result;
	}

	vector<Actor*> find_actors_low() {
		vector<Actor*> result;

		auto process_list = ProcessLists::GetSingleton();
		for (const ActorHandle& actor_handle: process_list->lowActorHandles)
		{
			if (!actor_handle) {
				continue;
			}
			auto actor_smartptr = actor_handle.get();
			if (!actor_smartptr) {
				continue;
			}

			Actor* actor = actor_smartptr.get();
			// auto actor = actor_handle.get().get();
			if (actor && actor->Is3DLoaded()) {
				result.push_back(actor);
			}
		}
		return result;
	}*/

	vector<Actor*> FindSomeActors(std::string_view tag, uint32_t howMany) {
		static unordered_map<string, FindActorData> allData;
		allData.try_emplace(string(tag));
		auto& data = allData.at(string(tag));

		//log::info("Looking for actor for {} up to a count of {}", tag, howMany);
		vector<Actor*> finalActors;
		vector<Actor*> notAddedAcrors;
		uint32_t addedCount = 0;
		for (const auto actor: find_actors_high()) {
			// Player or teammate are always updated
			if (actor->formID == 0x14 || IsTeammate(actor)) {
				finalActors.push_back(actor);
				//log::info(" - Adding: {}", actor->GetDisplayFullName());
			} else if (!data.previousActors.contains(actor->formID) && (addedCount < howMany)) {
				// Other actors are only added if they are not in the previous actor list

				//log::info(" - Adding: {}", actor->GetDisplayFullName());
				finalActors.push_back(actor);
				data.previousActors.insert(actor->formID);
				addedCount += 1;

			} else {
				notAddedAcrors.push_back(actor);
			}
		}
		// Reached the end of all actor
		if (addedCount < howMany) {
			// We need more. Reset the used list and add from
			// those not added set
			data.previousActors.clear();
			for (auto actor: notAddedAcrors) {
				if (addedCount < howMany) {
					finalActors.push_back(actor);
					data.previousActors.insert(actor->formID);
					addedCount += 1;
				} else {
					break;
				}
			}
		}
		return finalActors;
	}

	vector<Actor*> FindTeammates() {
		vector<Actor*> finalActors;
		for (auto actor: find_actors()) {
			if (IsTeammate(actor)) {
				finalActors.push_back(actor);
			}
		}
		return finalActors;
	}

	vector<Actor*> FindFemaleTeammates() {
		vector<Actor*> finalActors;
		for (auto actor : find_actors()) {
			if (IsTeammate(actor) && IsFemale(actor)) {
				finalActors.push_back(actor);
			}
		}
		return finalActors;
	}
}