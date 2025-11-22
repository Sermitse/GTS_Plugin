#pragma once

namespace GTS {

	class ButtCrushController : public EventListener, public CInitSingleton <ButtCrushController> {

		public:
		virtual std::string DebugName() override;

		static void ButtCrush_OnCooldownMessage(Actor* giant);
		std::vector<Actor*> GetButtCrushTargets(Actor* pred, std::size_t numberOfPrey);
		bool CanButtCrush(Actor* pred, Actor* prey) const;
		static void StartButtCrush(Actor* pred, Actor* prey, bool dochecks = true);
		void AllowMessage(bool allow);

		private:
		bool allow_message = false;
	};
}