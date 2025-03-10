#pragma once

namespace GTS {

	class SandwichingData {
		public:
			SandwichingData(Actor* giant);
			// Adds a tiny to the list of actors
			// being eaten
			void AddTiny(Actor* tiny);
			void Remove(Actor* tiny);
			void EnableSuffocate(bool enable);
			void ManageScaleRune(bool enable);
			void ManageShrinkRune(bool enable);
			void OverideShrinkRune(float value);
			// Release all vories (shall fall into mouth with animation)
			void ReleaseAll();

			// Get a list of all actors currently being vored
			std::vector<Actor*> GetActors();

			// Update all things that are happening like
			// keeping them on the AnimObjectA and shrinking nodes
			static void EnableRuneTask(Actor* a_Giant, bool shrink);
			static void DisableRuneTask(Actor* a_Giant, bool shrink);
			void Update();
			void MoveActors(bool move);

		private:
			ActorHandle giant;
			// Vore is done is sets with multiple actors if the giant is big
			// enough
			std::unordered_map<FormID, ActorHandle> tinies = {};
			bool MoveTinies = false;
			bool Suffocate = false;
			bool RuneScale = false;
			bool RuneShrink = false;

			Spring ScaleRune = Spring(0.0f, 1.5f);
			Spring ShrinkRune = Spring(0.0f, 1.5f);
			inline static Timer SandwichTimer = Timer(0.45);


			// True if in grabbed state
			bool allGrabbed = false;
	};
	class ThighSandwichController : public EventListener  {
		public:
			[[nodiscard]] static ThighSandwichController& GetSingleton() noexcept;

			virtual std::string DebugName() override;
			virtual void Update() override;
			virtual void Reset() override;
			virtual void ResetActor(Actor* actor) override;

			std::vector<Actor*> GetSandwichTargetsInFront(Actor* pred, std::size_t numberOfPrey);
			static void StartSandwiching(Actor* pred, Actor* prey, bool dochecks = true);
			bool CanSandwich(Actor* pred, Actor* prey) const;

			SandwichingData& GetSandwichingData(Actor* giant);

			std::unordered_map<FormID, SandwichingData> data;
			void AllowMessage(bool allow);
		private:
			bool allow_message = false;
	};
}
