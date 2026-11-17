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
			

			//The game recycles controller allocations, so an entry may still be present for a destroyed controller at this address.
			//Built before taking MapLock: the constructor takes the world lock, and a controller destructor may hold
			//that one while it waits for MapLock.
			if (RE::bhkCharacterController* controller = a_actor->GetCharController()) {
				auto instance = std::make_shared<DynamicCollisionController>(a_actor->GetHandle(), controller, hasPreExistingCollisionShape);
				WriteLock lock(MapLock);
				auto [it, inserted] = ControllerMap.try_emplace(controller, instance);
				if (!inserted) {
					//An update snapshot may still hold the old instance, and the identity check passes on a recycled address.
					it->second->MarkDestroyed();
					it->second = std::move(instance);
				}
			}
		}
	}

	void DynamicCollisionManager::DestroyInstance(RE::bhkCharacterController* a_controller) {
		WriteLock lock(MapLock);
		if (auto it = ControllerMap.find(a_controller); it != ControllerMap.end()) {
			it->second->MarkDestroyed();
			ControllerMap.erase(it);
		}
	}

	void DynamicCollisionManager::Update() {

		if (!State::InGame()) return;

		//Updates run on a snapshot so MapLock is never held while a controller takes the world lock.
		//A controller destructor may run under the world lock and then wait for MapLock, and the
		//update may also release the last reference to a controller itself.
		absl::InlinedVector<std::shared_ptr<DynamicCollisionController>, 32> controllers;
		{
			ReadLock lock(MapLock);
			controllers.reserve(ControllerMap.size());
			for (const std::shared_ptr<DynamicCollisionController>& DynController : ControllerMap | std::views::values) {
				controllers.push_back(DynController);
			}
		}

		for (const std::shared_ptr<DynamicCollisionController>& DynController : controllers) {
			DynController->Update();
		}
	}

	std::string DynamicCollisionManager::DebugName() {
		return "::DynamicCollisionManager";
	}
}
