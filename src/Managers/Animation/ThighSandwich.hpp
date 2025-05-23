#pragma once

namespace GTS {

	class AnimationThighSandwich : public EventListener {
		public:
			[[nodiscard]] static AnimationThighSandwich& GetSingleton() noexcept;

			virtual std::string DebugName() override;

			static void GrabActor(Actor* giant, TESObjectREFR* tiny);
			static void Release(Actor* giant);
			// Get object being held
			static TESObjectREFR* GetHeldObj(Actor* giant);
			static Actor* GetHeldActor(Actor* giant);

			std::vector<Actor*> GetSandwichTargetsInFront(Actor* pred, std::size_t numberOfPrey);
			static void StartSandwiching(Actor* pred, Actor* prey);
			static void CanSandwich(Actor* pred, Actor* prey);

			static void RegisterEvents();

			static void RegisterTriggers();
	};
}
