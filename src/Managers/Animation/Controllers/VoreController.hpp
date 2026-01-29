#pragma once

namespace GTS {

	// Represents current vore data for an actor
	class VoreData {
		public:
			VoreData(Actor* giant);
			// Adds a tiny to the list of actors
			// being eaten
			void AddTiny(Actor* tiny);
			// Enables/diables the shrink zone
			void EnableMouthShrinkZone(bool enabled);
			// Swallow and start the digestion (buffs)
			void Swallow();
			// Finishes the process
			// kill/shrinks all actors and gains buffs
			void KillAll();
			// Protects from being vored. Used to disallow same-target vore by 2 actors
			void AllowToBeVored(bool allow);

			// Grab all vories
			void GrabAll();

			// Release all vories (shall fall into mouth with animation)
			void ReleaseAll();

			bool GetTimer();

			// Get a list of all actors currently being vored
			std::vector<Actor*> GetVories();

			// Update all things that are happening like
			// keeping them on the AnimObjectA and shrinking nodes
			void Update();

		private:
			ActorHandle giant;
			// Vore is done is sets with multiple actors if the giant is big
			// enough
			mutable std::mutex _lock;
			absl::flat_hash_map<FormID, ActorHandle> tinies = {};

			// If true the mouth kill zone is on and we shrink nodes entering the mouth
			bool killZoneEnabled = false;

			inline static Timer moantimer = Timer(6.0);

			// True if in grabbed state
			bool allGrabbed = false;
	};

	class VoreController : public EventListener, public CInitSingleton <VoreController> {
		public:
		virtual std::string DebugName() override;
		virtual void Reset() override;
		virtual void ResetActor(Actor* actor) override;
		virtual void Update() override;

		// Get's vore target for any actor based on direction they are facing
		// This will only return actors with appropiate distance/scale
		// as based on `CanVore`
		Actor* GetVoreTargetInFront(Actor* pred);

		// Get's vore target for any actor based on distance from pred
		// This will only return actors with appropiate distance/scale
		// as based on `CanVore`  and can return multiple targets
		std::vector<Actor*> GetVoreTargetsInFront(Actor* pred, std::size_t numberOfPrey);

		// Check if they can vore based on size difference and reach distance
		bool CanVore(Actor* pred, Actor* prey) const;

		// Do the vore
		void StartVore(Actor* pred, Actor* prey);

		static void RecordOriginalScale(Actor* tiny);
		static float ReadOriginalScale(Actor* tiny);
		static void ShrinkOverTime(Actor* giant, Actor* tiny, float time_mult = 1.0f, float targetscale_mult = 1.0f);
		
		// Gets the current vore data of a giant
		VoreData& GetVoreData(Actor* giant);

		void AllowMessage(bool allow);

		[[nodiscard]] bool IsTinyInDataList(Actor* aTiny);

		private:
		std::unordered_map<FormID, VoreData> data;
		mutable std::mutex _lock;
		bool allow_message = false;
	};
}
