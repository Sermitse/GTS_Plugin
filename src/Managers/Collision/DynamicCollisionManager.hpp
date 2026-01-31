#pragma once 
#include "Managers/Collision/DynamicCollisionController.hpp"

namespace GTS {

	class DynamicCollisionManager : public CInitSingleton<DynamicCollisionManager>, public EventListener {

		public:
		static void DestroyInstance(RE::bhkCharacterController* a_controller);
		static void CreateInstance(RE::Actor* a_actor);

		private:
		virtual void Update() override;

		using Lock = std::shared_mutex;
		using ReadLock = std::shared_lock<Lock>;
		using WriteLock = std::unique_lock<Lock>;

		static inline Lock MapLock;
		static inline absl::flat_hash_map<bhkCharacterController*, std::shared_ptr<DynamicCollisionController>> ControllerMap = {};

		// Inherited via EventListener
		std::string DebugName() override;
	};

}