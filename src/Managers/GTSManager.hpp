#pragma once

namespace GTS {

	class GTSManager : public EventListener, public CInitSingleton <GTSManager> {

		public:
		virtual void DragonSoulAbsorption() override;
		virtual std::string DebugName() override;
		virtual void Start() override;
		virtual void Update() override;
		virtual void OnGameLoaded() override;


		//Used for profiling
		static inline uint32_t LoadedActorCount = 0;

		// Reapply changes (used after reload events)
		static void reapply(bool force = true);
		static void reapply_actor(Actor* actor, bool force = true);
	};
}
