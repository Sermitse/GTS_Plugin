#include "RE/H/hkpWorld.hpp"

namespace RE {

	void UpdateCollisionFilterOnEntity(
		hkpWorld* world, 
		hkpEntity* entity, 
		hkpUpdateCollisionFilterOnEntityMode updateMode, 
		hkpUpdateCollectionFilterMode updateShapeCollectionFilter) {

		typedef void (
			*DefUpdateCollisionFilterOnEntity)(
			hkpWorld* world, 
			hkpEntity* entity, 
			hkpUpdateCollisionFilterOnEntityMode updateMode, 
			hkpUpdateCollectionFilterMode updateShapeCollectionFilter
			);

		REL::Relocation<DefUpdateCollisionFilterOnEntity> RealUpdateCollisionFilterOnEntity{ RELOCATION_ID(60509, 61321) };
		RealUpdateCollisionFilterOnEntity(world, entity, updateMode, updateShapeCollectionFilter);
	}

	void UpdateCollisionFilterOnPhantom(
		hkpWorld* world, 
		hkpPhantom* phantom, 
		hkpUpdateCollectionFilterMode updateShapeCollectionFilter) {

		typedef void (
			*DefUpdateCollisionFilterOnPhantom)(hkpWorld* world,
			hkpPhantom* entity, 
			hkpUpdateCollectionFilterMode updateShapeCollectionFilter
			);

		REL::Relocation<DefUpdateCollisionFilterOnPhantom> RealUpdateCollisionFilterOnPhantom{ RELOCATION_ID(60509, 61321) };
		RealUpdateCollisionFilterOnPhantom(world, phantom, updateShapeCollectionFilter);
	}

}