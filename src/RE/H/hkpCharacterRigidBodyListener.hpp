#pragma once

#include "RE/H/hkpWorld.hpp"
#include "RE/H/hkpCharacterRigidBody.hpp"

namespace RE {

	class hkpSimpleConstraintContactMgr;

	class hkpCharacterRigidBodyListener : public hkReferencedObject {
		public:
		inline static constexpr auto RTTI = RTTI_hkpCharacterRigidBodyListener;

		virtual ~hkpCharacterRigidBodyListener() override;
		virtual void characterCallback(hkpWorld* world, hkpCharacterRigidBody* characterRB);
		virtual void processActualPoints(const hkpWorld* world, hkpCharacterRigidBody* characterRB, const hkpLinkedCollidable::CollisionEntry& entry, hkpSimpleConstraintContactMgr* mgr, hkArray<std::uint16_t>& contactPointIds);
		virtual void unweldContactPoints(hkpCharacterRigidBody* characterRB, const hkpLinkedCollidable::CollisionEntry& entry, hkpSimpleConstraintContactMgr* mgr, const hkArray<std::uint16_t>& contactPointIds);
		virtual void considerCollisionEntryForSlope(const hkpWorld* world, hkpCharacterRigidBody* characterRB, const hkpLinkedCollidable::CollisionEntry& entry, hkpSimpleConstraintContactMgr* mgr, hkArray<std::uint16_t>& contactPointIds);
		virtual void considerCollisionEntryForMassModification(const hkpWorld* world, hkpCharacterRigidBody* characterRB, const hkpLinkedCollidable::CollisionEntry& entry, hkpSimpleConstraintContactMgr* mgr, const hkArray<std::uint16_t>& contactPointIds);
	};


}