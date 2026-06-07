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

		bool CheckFlags() const {
			return (((flags & 0x70000000) - 0x30000000) & 0xEFFFFFFF) == 0;
		}
	};

	bool CheckSkeletonForCollisionShapes(RE::NiAVObject* Object) {
		
		if (!Object) {
			return false;
		}

		if (RE::NiPointer<RE::NiCollisionObject>& collisionObject = Object->collisionObject) {
			if (RE::bhkNiCollisionObject* bhkCollisionObject = collisionObject->AsBhkNiCollisionObject()) {
				if (bhkCollisionObject->body) {
					if (RE::hkRefPtr<RE::hkReferencedObject>& referencedObject = bhkCollisionObject->body->referencedObject) {
						if (RE::hkpWorldObject* worldObject = static_cast<RE::hkpWorldObject*>(referencedObject.get())) {
							if (const RE::hkpCollidable* collidable = worldObject->GetCollidable()) {
								if (collidable->shape && collidable->shape->userData) {
									RE::COL_LAYER layer = static_cast<RE::COL_LAYER>(collidable->broadPhaseHandle.collisionFilterInfo & 0x7F);
									if (layer == RE::COL_LAYER::kCharController) {
										if (collidable->shape->userData->materialID != RE::MATERIAL_ID::kCharacterBumper) {
											return true;
										}
									}
								}
							}
						}
					}
				}
			}
		}

		if (auto node = Object->AsNode()) {
			if (node->children.size() > 0) {
				for (auto& child : node->children) {
					if (CheckSkeletonForCollisionShapes(child.get())) {
						return true;
					}
				}
			}
		}
		return false;
	}
}


namespace GTS {

	void DynamicCollisionManager::CreateInstance(RE::Actor* a_actor) {

		if (a_actor) {

			bool hasPreExistingCollisionShape = false;

			if (auto loadedData = a_actor->loadedData) {
				RE::NiAVObject* object = loadedData->data3D.get();
				if (loadedData->unk60 && loadedData->unk58) {
					if (auto pUnk58 = reinterpret_cast<Unk58**>(loadedData->unk58)) {
						auto unk58 = *pUnk58;
						if (unk58->CheckFlags()) {
							object = unk58->object;
						}
					}
				}

				hasPreExistingCollisionShape = CheckSkeletonForCollisionShapes(object);
			}
			

			{
				WriteLock lock(MapLock);
				ControllerMap.try_emplace(a_actor->GetCharController(), std::make_shared<DynamicCollisionController>(a_actor->GetHandle(), hasPreExistingCollisionShape));
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
