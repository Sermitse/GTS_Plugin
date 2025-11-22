#pragma once

// Module that handles footsteps

namespace GTS {

	class StompManager {
		public:
		static void PlayNewOrOldStomps(Actor* giant, float modifier, FootEvent foot_kind, std::string_view find_foot, bool Strong);
		static void PlayStompSounds(Actor* giant, float modifier, std::string_view find_foot, FootEvent foot_kind, float scale, bool Strong);
		static float Volume_Multiply_Function(Actor* actor, FootEvent Kind);
	};
}
