#pragma once

namespace GTS {

	class Animation_TinyCalamity {
		public:
		static void RegisterEvents();
        static void RegisterTriggers();
		static void ResetActors(Actor* actor);
        static void AddToData(Actor* giant, Actor* tiny, float until);
        static std::vector<Actor*> GetShrinkActors(Actor* giant);
        static float GetShrinkUntil(Actor* giant);
    };
}

