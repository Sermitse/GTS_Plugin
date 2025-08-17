#pragma once

namespace GTS {
	/**
	 * The class which tracks gts size effects.
	 */
	class GtsManager : public EventListener  {
		public:

			/**
			 * Get the singleton instance of the <code>GtsManager</code>.
			 */
			[[nodiscard]] static GtsManager& GetSingleton() noexcept;

			float experiment = 1.0f;

			//Used for profiling
			static inline uint32_t LoadedActorCount = 0;

			virtual void DragonSoulAbsorption() override;

			virtual std::string DebugName() override;
			virtual void Start() override;
			virtual void Update() override;

			// Reapply changes (used after reload events)
			static void reapply(bool force = true);
			static void reapply_actor(Actor* actor, bool force = true);
	};
}
