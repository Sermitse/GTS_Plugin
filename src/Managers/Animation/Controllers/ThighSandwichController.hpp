#pragma once

namespace GTS {

	enum class RuneTask {
		kShrink,
		kEnlarge,
	};

	class SandwichingData {
		public:
		SandwichingData(Actor* giant);
		// Adds a tiny to the list of actors
		// being eaten
		void AddTiny(Actor* tiny);
		void Remove(Actor* tiny);
		void EnableSuffocate(bool enable);
		void SetSuffocateMult(float mult);
		// Release all vories (shall fall into mouth with animation)
		void ReleaseAll();

		// Get a list of all actors currently being vored
		std::vector<Actor*> GetActors();

		// Update all things that are happening like
		// keeping them on the AnimObjectA and shrinking nodes
		static void StartRuneTask(Actor* a_Giant, RuneTask Type);
		void Update();
		void MoveActors(bool move);

		private:
		ActorHandle giant;
		// Vore is done is sets with multiple actors if the giant is big
		// enough
		absl::node_hash_map<FormID, ActorHandle> tinies = {};
		bool MoveTinies = false;
		bool Suffocate = false;
		float SuffocateMult = 1.0f;

		Spring ScaleRune = Spring(0.0f, 1.5f);
		Spring ShrinkRune = Spring(0.0f, 1.5f);
		inline static Timer SandwichTimer = Timer(0.45);


		// True if in grabbed state
		bool allGrabbed = false;
	};
	class ThighSandwichController : public EventListener, public CInitSingleton <ThighSandwichController> {
		public:
		virtual std::string DebugName() override;
		virtual void Update() override;
		virtual void Reset() override;
		virtual void ResetActor(Actor* actor) override;
		std::vector<Actor*> GetSandwichTargetsInFront(Actor* pred, std::size_t numberOfPrey);
		static void StartSandwiching(Actor* pred, Actor* prey, bool dochecks = true);
		bool CanSandwich(Actor* pred, Actor* prey) const;
		SandwichingData& GetSandwichingData(Actor* giant);
		absl::node_hash_map<FormID, SandwichingData> data;
		void AllowMessage(bool allow);

		private:
		bool allow_message = false;
	};
}
