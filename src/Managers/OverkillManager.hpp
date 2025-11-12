#pragma once

// Module that handles overkilling others

namespace GTS {

	enum class OverkillState {
		Healthy,
		Overkilling,
		Overkilled
	};

	class OverkillData {
		public:
			OverkillData(Actor* giant);

			OverkillState state;
			Timer delay;
			ActorHandle giant;
	};

	class OverkillManager : public EventListener, public CInitSingleton<OverkillManager> {
		public:
		virtual std::string DebugName() override;
		virtual void Update() override;
		virtual void Reset() override;
		virtual void ResetActor(Actor* actor) override;
		static bool CanOverkill(Actor* giant, Actor* tiny);
		static bool AlreadyOverkilled(Actor* actor);
		static void Overkill(Actor* giant, Actor* tiny);
		private:
			std::unordered_map<FormID, OverkillData> data;
	};
}