#include "RE/B/bhkRigidBody.hpp"

namespace RE {

	void SetLinearImpulse(bhkRigidBody* a_rigidBody, const hkVector4& a_impulse) {
		using func_t = decltype(&SetLinearImpulse);
		static const REL::Relocation<func_t> func{ REL::RelocationID(76261, 78091, NULL) };
		return func(a_rigidBody, a_impulse);
	}

	void SetAngularImpulse(bhkRigidBody* a_rigidBody, const hkVector4& a_impulse) {
		using func_t = decltype(&SetAngularImpulse);
		static const REL::Relocation<func_t> func{ REL::RelocationID(76262, 78092, NULL) };
		return func(a_rigidBody, a_impulse);
	}

}