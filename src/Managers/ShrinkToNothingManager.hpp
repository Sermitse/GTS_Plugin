#pragma once

// Module that handles shrinking to nothing

namespace GTS {

	enum class ShrinkState {
		Healthy,
		Shrinking,
		Shrinked
	};

	class ShrinkData {
		public:
			ShrinkData(Actor* giant);

			ShrinkState state;
			Timer delay;
			ActorHandle giant;
	};

	class ShrinkToNothingManager : public EventListener, public CInitSingleton <ShrinkToNothingManager> {
		public:
		virtual std::string DebugName() override;
		virtual void Update() override;
		virtual void Reset() override;
		virtual void ResetActor(Actor* actor) override;

		static bool CanShrink(Actor* giant, Actor* tiny);
		static bool AlreadyShrinked(Actor* actor);
		static void Shrink(Actor* giant, Actor* tiny);

		static void SpawnDeathEffects(Actor* tiny);
		static void TransferInventoryTask(Actor* giant, Actor* tiny);

		private:
		absl::node_hash_map<FormID, ShrinkData> data;
	};
}
