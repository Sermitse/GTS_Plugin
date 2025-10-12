#include "RE/H/hkpRigidBody.hpp"

namespace RE {

	void SetMotionType(
		hkpRigidBody* a_this, 
		hkpMotion::MotionType a_newState, 
		hkpEntityActivation a_preferredActivationState, 
		hkpUpdateCollisionFilterOnEntityMode a_collisionFilterUpdateMode) {

		typedef void (
			*DefRealSetMotionType)(
			hkpRigidBody* a_this, 
			hkpMotion::MotionType a_newState, 
			hkpEntityActivation a_preferredActivationState, 
			hkpUpdateCollisionFilterOnEntityMode a_collisionFilterUpdateMode
			);

		REL::Relocation<DefRealSetMotionType> RealSetMotionType{ RELOCATION_ID(60153, 60908) };
		RealSetMotionType(a_this, a_newState, a_preferredActivationState, a_collisionFilterUpdateMode);
	}
}