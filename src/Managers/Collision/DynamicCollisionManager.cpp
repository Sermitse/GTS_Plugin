#include "Managers/Collision/DynamicCollisionManager.hpp"

namespace GTS {

	void DynamicCollisionManager::CreateInstance(RE::Actor* a_actor) {
		WriteLock lock(MapLock);
		ControllerMap.try_emplace(a_actor->GetCharController(), std::make_shared<DynamicCollisionController>(a_actor->GetHandle()));
	}

	void DynamicCollisionManager::DestroyInstance(RE::bhkCharacterController* a_controller) {
		WriteLock lock(MapLock);
		ControllerMap.erase(a_controller);
	}

	void DynamicCollisionManager::Update() {

		if (!State::InGame()) return;

		{
			ReadLock lock(MapLock);
			for (const std::shared_ptr<DynamicCollisionController>& DynController : ControllerMap | std::views::values) {
				DynController->Update();
			}
		}
	}

	std::string DynamicCollisionManager::DebugName() {
		return "::DynamicCollisionManager";
	}
}
