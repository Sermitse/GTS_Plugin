#include "Managers/Collision/DynamicCollisionManager.hpp"

#include "DynamicCollisionUtils.hpp"

namespace {

	struct Unk58 {

		uint64_t unk00;
		uint32_t unk08;
		uint32_t flags;
		uint64_t unk10;
		uint64_t unk18;
		uint64_t unk20;
		RE::NiAVObject* object;

		bool CheckFlags() const {
			return (((flags & 0x70000000) - 0x30000000) & 0xEFFFFFFF) == 0;
		}
	};

	RE::NiAVObject* GetActorSkeleton(RE::Actor* a_actor) {
		if (a_actor) {
			if (RE::LOADED_REF_DATA* loadedData = a_actor->loadedData) {
				RE::NiAVObject* object = loadedData->data3D.get();
				if (loadedData->unk60 && loadedData->unk58) {
					if (Unk58** pUnk58 = reinterpret_cast<Unk58**>(loadedData->unk58)) {
						Unk58* unk58 = *pUnk58;
						if (unk58->CheckFlags()) {
							object = unk58->object;
						}
					}
				}
				return object;
			}
		}
		return nullptr;
	}

}

namespace GTS {

	void DynamicCollisionManager::CreateInstance(RE::Actor* a_actor) {
		if (GetActorSkeleton(a_actor)) { // Make sure the actor has a skeleton, This check should not be needed, but it could be possible that somehow an actor without a skeleton gets instantiated.
			WriteLock lock(MapLock);
			ControllerMap.try_emplace(a_actor->GetCharController(), std::make_shared<DynamicController>(a_actor->GetHandle()));
		}
	}

	void DynamicCollisionManager::DestroyInstance(RE::bhkCharacterController* a_controller) {
		WriteLock lock(MapLock);
		ControllerMap.erase(a_controller);
	}

	void DynamicCollisionManager::Update() {

		if (!State::InGame()) return;

		if (!UI::GetSingleton()->GameIsPaused()) {
			if (DebugDraw::CanDraw()) {
				DrawCollisionShapes(PlayerCharacter::GetSingleton()->GetCharController(), PlayerCharacter::GetSingleton());
				for (auto find_actor : find_actors()) {
					DrawCollisionShapes(find_actor->GetCharController(), find_actor);
				}
			}
		}

		{
			ReadLock lock(MapLock);
			for (const shared_ptr<DynamicController>& DynController : ControllerMap | views::values) {
				DynController->Update();
			}
		}
	}

	std::string DynamicCollisionManager::DebugName() {
		return "::DynamicCollisionManager";
	}
}
