#include "Systems/Colliders/CollidesWith.hpp"

namespace GTS {

	COL_LAYER GetCollidesWith(const std::uint32_t& collisionFilterInfo) {
		return static_cast<COL_LAYER>(collisionFilterInfo & 0x7F);
	}

	COL_LAYER GetCollidesWith(const hkpCollidable* collidable) {
		if (collidable) {
			return GetCollidesWith(collidable->broadPhaseHandle.collisionFilterInfo);
		}
		else {
			return COL_LAYER::kUnidentified;
		}
	}

	COL_LAYER GetCollidesWith(const hkpWorldObject* entity) {
		if (entity) {
			auto collidable = entity->GetCollidable();
			return GetCollidesWith(collidable);
		}
		else {
			return COL_LAYER::kUnidentified;
		}
	}

	void SetCollidesWith(std::uint32_t& collisionFilterInfo, const COL_LAYER& newLayer) {
		auto newCollision = collisionFilterInfo & 0xFFFFFF80; // Clear old one
		newCollision = newCollision | static_cast<std::uint32_t>(newLayer);
		collisionFilterInfo = newCollision;
	}

	void SetCollidesWith(hkpCollidable* collidable, const COL_LAYER& newLayer) {
		if (collidable) {
			return SetCollidesWith(collidable->broadPhaseHandle.collisionFilterInfo, newLayer);
		}
	}

	void SetCollidesWith(hkpWorldObject* entity, const COL_LAYER& newLayer) {
		if (entity) {
			auto collidable = entity->GetCollidableRW();
			return SetCollidesWith(collidable, newLayer);
		}
	}
}