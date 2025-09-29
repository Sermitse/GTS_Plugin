#include "Utils/FindActor.hpp"

namespace GTS {

	/**
	 * Find actors using the AIManager process lists.
	 */

	inline vector<Actor*> find_actors() {

		GTS_PROFILE_SCOPE("FindActor: FindActors");

		const ProcessLists* const process_list = ProcessLists::GetSingleton();
		const auto& handles = process_list->highActorHandles;
		const std::size_t n = handles.size();

		std::vector<Actor*> result;
		result.reserve(n + 1);

		for (std::size_t i = 0; i < n; ++i) {
			if (const auto& a = handles[i].get(); a && a->Get3D1(false)) {
				result.emplace_back(a.get());
			}
		}

		if (Actor* player = PlayerCharacter::GetSingleton(); player && player->Get3D1(false)) {
			result.emplace_back(player);
		}

		return result;
	}


	// This will find up to howMany actors in the scene
	// (not including player and teammate which are ALWAYS returned
	// regardless of howMany are asked for)
	//
	// Any actors not found in this call will instead be returned on next call
	// This means that in frame 1 you can get 10 actors + player team
	// In frame 2 you will get 10 DIFFERENT actors + player team
	// Until all actors have been returned after which you will get previous actors again
	vector<Actor*> FindSomeActors(std::string_view tag, uint32_t howMany) {

		static unordered_map<string, FindActorData> allData;
		allData.try_emplace(string(tag));
		auto& data = allData.at(string(tag));

		//log::info("Looking for actor for {} up to a count of {}", tag, howMany);
		vector<Actor*> finalActors;
		vector<Actor*> notAddedAcrors;
		uint32_t addedCount = 0;

		for (const auto actor: find_actors()) {
			// Player or teammate are always updated
			if (actor->formID == 0x14 || IsTeammate(actor)) {
				finalActors.push_back(actor);
				//log::info(" - Adding: {}", actor->GetDisplayFullName());
			}
			else if (!data.previousActors.contains(actor->formID) && (addedCount < howMany)) {
				// Other actors are only added if they are not in the previous actor list

				//log::info(" - Adding: {}", actor->GetDisplayFullName());
				finalActors.push_back(actor);
				data.previousActors.insert(actor->formID);
				addedCount += 1;

			}
			else {
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

	// Find player teammates
	// But not the player themselves
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