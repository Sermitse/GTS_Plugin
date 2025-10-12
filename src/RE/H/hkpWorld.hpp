#pragma once

#include "RE/H/hkpRigidBody.hpp"
#include "RE/H/hkpWorld.h"
#include "RE/H/hkpEntity.hpp"

namespace RE {

	// SSE: 60509, AE: 60908
	// 55 53 41 54 41 55 41 56 48 8d ac 24 90 f0 ff ff
	void UpdateCollisionFilterOnEntity(hkpWorld* world, hkpEntity* entity, hkpUpdateCollisionFilterOnEntityMode updateMode, hkpUpdateCollectionFilterMode updateShapeCollectionFilter);

	// SSE: 60511, AE: 61323
	// 40 55 53 41 54 41 56 48 8b ec 48 83 ec 68 83 b9 f8 00 00 00 00
	void UpdateCollisionFilterOnPhantom(hkpWorld* world, hkpPhantom* phantom, hkpUpdateCollectionFilterMode updateShapeCollectionFilter);
}