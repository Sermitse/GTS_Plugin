#include "Colliders/ColliderData.hpp"
#include "Colliders/CollidesWith.hpp"

namespace GTS {

	

	void ColliderData::Activate() {
		log::info("Activate RBs");
		for (auto rb: GetRigidBodies()) {
			log::info("  - Activating");
			SetMotionType(rb, hkpMotion::MotionType::kCharacter, hkpEntityActivation::kDoActivate, hkpUpdateCollisionFilterOnEntityMode::kFullCheck);
		}
	}

	void ColliderData::UpdateCollisionFilter() {
		for (auto ent: GetRigidBodies()) {
			if (ent) {
				if (ent->world) {
					UpdateCollisionFilterOnEntity(ent->world, ent, hkpUpdateCollisionFilterOnEntityMode::kFullCheck, hkpUpdateCollectionFilterMode::kIncludeCollections);
				}
			}
		}
		for (auto ent: GetPhantoms()) {
			if (ent) {
				if (ent->world) {
					UpdateCollisionFilterOnPhantom(ent->world, ent, hkpUpdateCollectionFilterMode::kIncludeCollections);
				}
			}
		}
	}

	void ColliderData::AddRB(hkpRigidBody* rb) {
		if (!rb) {
			return;
		}
		this->rbs.try_emplace(rb, hkRefPtr(rb));
	}

	void ColliderData::AddPhantom(hkpPhantom* phantom) {
		if (!phantom) {
			return;
		}
		this->phantoms.try_emplace(phantom, hkRefPtr(phantom));
	}

	std::vector<ColliderData*> ColliderData::GetChildren() {
		return {};
	}

	std::vector<hkpRigidBody*> ColliderData::GetRigidBodies() {
		std::vector<hkpRigidBody*> entities = {};
		for (auto& rb : this->rbs | views::values) {
			entities.push_back(rb.get());
		}
		for (auto& child: GetChildren()) {
			for (auto& ent: child->GetRigidBodies()) {
				entities.push_back(ent);
			}
		}
		return entities;
	}
	
	std::vector<hkpPhantom*> ColliderData::GetPhantoms() {
		std::vector<hkpPhantom*> entities = {};
		for (auto& ph : this->phantoms | views::values) {
			entities.push_back(ph.get());
		}
		for (auto& child: GetChildren()) {
			for (auto& ent: child->GetPhantoms()) {
				entities.push_back(ent);
			}
		}
		return entities;
	}

	std::vector<hkpWorldObject*> ColliderData::GetWorldObjects() {
		std::vector<hkpWorldObject*> entities = {};
		for (auto& rb: GetRigidBodies()) {
			entities.push_back(rb);
		}
		for (auto& ph: GetPhantoms()) {
			entities.push_back(ph);
		}
		return entities;
	}

	void ColliderData::DisableCollisions() {
		std::vector<hkpWorldObject*> entities = GetWorldObjects();
		for (auto& rb: GetRigidBodies()) {
			// Disable gravity
			// log::info("Disable gravity (was {})", rb->motion.gravityFactor);
			rb->motion.gravityFactor = 0.0f;
			rb->motion.SetMassInv(0.0f);
		}

		for (auto& ent: GetWorldObjects()) {
			auto collidable = ent->GetCollidable();
			if (collidable) {
				// log::info("- Disable collision");
				// log::info("Current info: {:0X}", collidable->broadPhaseHandle.collisionFilterInfo);
				// log::info("        with: {:0X}", collidable->broadPhaseHandle.collisionFilterInfo & 0x7F);

				// Change collides with
				if (GetCollidesWith(ent) == COL_LAYER::kCharController) {
					SetCollidesWith(ent, COL_LAYER::kNonCollidable);
				}

				// log::info("    New info: {:0X}", collidable->broadPhaseHandle.collisionFilterInfo);
				// log::info("        with: {:0X}", collidable->broadPhaseHandle.collisionFilterInfo & 0x7F);
			}
		}
	}

	void ColliderData::EnableCollisions() {
		for (auto& rb: GetRigidBodies()) {
			// Enable gravity
			rb->motion.gravityFactor = 1.0f;
			rb->motion.SetMassInv(1.0f);
		}

		for (auto& ent: GetWorldObjects()) {
			auto collidable = ent->GetCollidable();
			if (collidable) {
				// log::info("- Enabling collision");
				// log::info("Current info: {:0X}", collidable->broadPhaseHandle.collisionFilterInfo);
				// log::info("        with: {:0X}", collidable->broadPhaseHandle.collisionFilterInfo & 0x7F);

				// Change collides with
				if (GetCollidesWith(ent) == COL_LAYER::kNonCollidable) {
					SetCollidesWith(ent, COL_LAYER::kCharController);
				}

				// log::info("    New info: {:0X}", collidable->broadPhaseHandle.collisionFilterInfo);
				// log::info("        with: {:0X}", collidable->broadPhaseHandle.collisionFilterInfo & 0x7F);
			}
		}
	}
}
