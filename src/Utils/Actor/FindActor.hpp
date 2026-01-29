#pragma once

namespace GTS {

	struct FindActorData final {
		/// Actors that have been done recently;
		absl::flat_hash_set<FormID> previousActors;
	};

	[[nodiscard]] std::vector<Actor*> __forceinline inline find_actors();
	[[nodiscard]] std::vector<Actor*> FindSomeActors(std::string_view tag, uint32_t howMany);
	[[nodiscard]] std::vector<Actor*> FindTeammates();
	[[nodiscard]] std::vector<Actor*> FindFemaleTeammates();

}