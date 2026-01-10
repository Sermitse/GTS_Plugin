#pragma once

namespace GTS {

	struct FindActorData final {
		/// Actors that have been done recently;
		unordered_set<FormID> previousActors;
	};

	[[nodiscard]] vector<Actor*> __forceinline inline find_actors();
	[[nodiscard]] vector<Actor*> FindSomeActors(std::string_view tag, uint32_t howMany);
	[[nodiscard]] vector<Actor*> FindTeammates();
	[[nodiscard]] vector<Actor*> FindFemaleTeammates();

}