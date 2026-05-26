#include "Managers/Collision/DynamicCollisionManager.hpp"

namespace {

	struct Unk58 {
		uint64_t unk00;
		uint32_t unk08;
		uint32_t flags;
		uint64_t unk10;
		uint64_t unk18;
		uint64_t unk20;
		RE::NiAVObject* object;

		inline bool CheckFlags() const {
			return (((flags & 0x70000000) - 0x30000000) & 0xEFFFFFFF) == 0;
		}
	};

	//Based on DCA's RE
	bool HasRuntimeCollisions(RE::Actor* a_actor) {
		if (!a_actor) return false;

		RE::LOADED_REF_DATA* loadedData = a_actor->loadedData;
		if (!loadedData) return false;

		if (loadedData->unk60 && loadedData->unk58) {
			if (auto pUnk58 = reinterpret_cast<Unk58**>(loadedData->unk58)) {
				auto unk58 = *pUnk58;
				if (unk58->CheckFlags()) {
					return true;
				}
			}
		}
		return false;
	}
}


namespace GTS {

	void DynamicCollisionManager::CreateInstance(RE::Actor* a_actor) {

		if (a_actor) {

			bool HasRuntimeCol = HasRuntimeCollisions(a_actor);

			{
				WriteLock lock(MapLock);
				ControllerMap.try_emplace(a_actor->GetCharController(), std::make_shared<DynamicCollisionController>(a_actor->GetHandle(), HasRuntimeCol && Runtime::HasKeyword(a_actor, Runtime::KYWD.CreatureKeyword)));
			}
		}
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
