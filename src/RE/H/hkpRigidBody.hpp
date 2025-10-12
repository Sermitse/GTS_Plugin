#pragma once

#include "RE/H/hkpEntity.hpp"
#include "RE/H/hkpRigidBody.h"

namespace RE {

	enum class hkpUpdateCollisionFilterOnEntityMode {
		kFullCheck,
		kDisableEntityEntityCollisionsOnly
	};

	enum class hkpUpdateCollectionFilterMode {
		kIgnoreCollections,
		kIncludeCollections,
	};

	void SetMotionType(hkpRigidBody* a_this, hkpMotion::MotionType a_newState, hkpEntityActivation a_preferredActivationState, hkpUpdateCollisionFilterOnEntityMode a_collisionFilterUpdateMode);
}