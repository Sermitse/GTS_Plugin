#pragma once

namespace GTS {

	[[nodiscard]] vector<Actor*> __forceinline inline find_actors();
	// This will find up to howMany actors in the scene
	// (not including player and teammate which are ALWAYS returned
	// regardless of howMany are asked for)
	//
	// Any actors not found in this call will instead be returned on next call
	// This means that in frame 1 you can get 10 actors + player team
	// In frame 2 you will get 10 DIFFERENT actors + player team
	// Until all actors have been returned after which you will get previous actors again
	vector<Actor*> FindSomeActors(std::string_view tag, uint32_t howMany);

	// Find player teammates
	// But not the player themselves
	vector<Actor*> FindTeammates();

	vector<Actor*> FindFemaleTeammates();

	static inline std::atomic<std::thread::id> main_update_thread_id{};
	static inline std::vector<Actor*> cached_actors;
	static inline std::mutex cache_mutex;
	static inline std::atomic<bool> cache_valid{ false };
}