#pragma once

namespace GTS {

	struct BoobCrushData {
		BoobCrushData(Actor* tiny);
		Actor* tiny;
	};

	class AnimationBoobCrush : public EventListener, public CInitSingleton <AnimationBoobCrush>{
		public:
		virtual std::string DebugName() override;
		virtual void Reset() override;
		virtual void ResetActor(Actor* actor) override;

		static void RegisterEvents();
		static void AttachActor(Actor* giant, Actor* tiny);
		static Actor* GetBoobCrushVictim(Actor* giant);
		static float GetBoobCrushDamage(Actor* actor);

		std::unordered_map<Actor*, BoobCrushData> data;
	};
}
