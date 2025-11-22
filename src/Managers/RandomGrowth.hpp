#pragma once

namespace GTS {

	class RandomGrowth : public EventListener, public CInitSingleton <RandomGrowth> {
		public:
		virtual std::string DebugName() override;
		virtual void Update() override;

		static void RestoreStats(Actor* actor, float multiplier);
	};
}
