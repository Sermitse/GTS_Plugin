#pragma once

namespace GTS {

	class ThighCrushController : public CInitSingleton <ThighCrushController> {
		public:
		std::vector<Actor*> GetThighTargetsInFront(Actor* pred, std::size_t numberOfPrey);
		bool CanThighCrush(Actor* pred, Actor* prey);
	};
}