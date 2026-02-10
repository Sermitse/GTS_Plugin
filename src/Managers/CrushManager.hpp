#pragma once

// Module that handles crushing others

namespace GTS {

	enum class CrushState {
		Healthy,
		Crushing,
		Crushed
	};

	class CrushData {
		public:
		CrushData(Actor* giant);
		CrushState state;
		Timer delay;
		ActorHandle giant;
	};

	class CrushManager : public EventListener, public CInitSingleton <CrushManager> {
		public:
		virtual std::string DebugName() override;
		virtual void Update() override;
		virtual void Reset() override;
		virtual void ResetActor(Actor* actor) override;

		static bool CanCrush(Actor* giant, Actor* tiny);
		static bool AlreadyCrushed(Actor* actor);
		static void Crush(Actor* giant, Actor* tiny);

		private:
		std::unordered_map<FormID, CrushData> data;
	};
}
