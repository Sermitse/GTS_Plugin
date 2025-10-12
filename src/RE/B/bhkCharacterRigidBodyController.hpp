#pragma once

#include "RE/B/bhkCharacterController.h"
#include "RE/B/bhkCharacterRigidBody.hpp"
#include "RE/B/bhkCharacterRigidBodyController.hpp"

#include "RE/H/hkpCharacterRigidBody.hpp"
#include "RE/H/hkpCharacterRigidBodyListener.hpp"
#include "RE/H/hkpLinkedCollidable.h"
#include "RE/H/hkpWorld.h"
#include "RE/H/hkVector4.h"

namespace RE {

	class hkpSimpleConstraintContactMgr;

	class bhkCharRigidBodyController :
		public bhkCharacterController, // 00
		public hkpCharacterRigidBodyListener { // 330

		public:
		inline static constexpr auto RTTI = RTTI_bhkCharRigidBodyController;

		~bhkCharRigidBodyController() override;  // 00

		// override (hkpCharacterRigidBodyListener)
		void characterCallback(hkpWorld* world, hkpCharacterRigidBody* characterRB) override;
		void processActualPoints(const hkpWorld* world, hkpCharacterRigidBody* characterRB, const hkpLinkedCollidable::CollisionEntry& entry, hkpSimpleConstraintContactMgr* mgr, hkArray<std::uint16_t>& contactPointIds) override;
		void unweldContactPoints(hkpCharacterRigidBody* characterRB, const hkpLinkedCollidable::CollisionEntry& entry, hkpSimpleConstraintContactMgr* mgr, const hkArray<std::uint16_t>& contactPointIds) override;
		void considerCollisionEntryForSlope(const hkpWorld* world, hkpCharacterRigidBody* characterRB, const hkpLinkedCollidable::CollisionEntry& entry, hkpSimpleConstraintContactMgr* mgr, hkArray<std::uint16_t>& contactPointIds) override;
		void considerCollisionEntryForMassModification(const hkpWorld* world, hkpCharacterRigidBody* characterRB, const hkpLinkedCollidable::CollisionEntry& entry, hkpSimpleConstraintContactMgr* mgr, const hkArray<std::uint16_t>& contactPointIds) override;

		// override (bhkCharacterController)
		void GetLinearVelocityImpl(hkVector4& a_velocity) const override;  // 06
		void SetLinearVelocityImpl(const hkVector4& a_velocity) override;  // 07

		bhkCharacterRigidBody characterRigidBody; // 340
	};
	static_assert(offsetof(bhkCharRigidBodyController, characterRigidBody) == 0x340);

}