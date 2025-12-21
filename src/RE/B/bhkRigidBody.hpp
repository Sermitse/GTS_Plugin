#pragma once

#include "RE/B/bhkRigidBody.h"
#include "RE/H/hkVector4.h"

namespace RE {

	void SetLinearImpulse(bhkRigidBody* a_rigidBody, const hkVector4& a_impulse);
	void SetAngularImpulse(bhkRigidBody* a_rigidBody, const hkVector4& a_impulse);

}