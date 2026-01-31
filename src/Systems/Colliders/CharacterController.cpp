#include "Systems/Colliders/CharacterController.hpp"

namespace GTS {

	CharacterControllerData::CharacterControllerData(bhkCharacterController* charCont) {

		if (!charCont) {
			return;
		}

		bhkCharProxyController* charProxyController = skyrim_cast<bhkCharProxyController*>(charCont);
		bhkCharRigidBodyController* charRigidBodyController = skyrim_cast<bhkCharRigidBodyController*>(charCont);
		if (charProxyController) {
			// Player controller is a proxy one
			auto& proxy = charProxyController->proxy;
			hkReferencedObject* refObject = proxy.referencedObject.get();
			if (refObject) {
				hkpCharacterProxy* hkpObject = skyrim_cast<hkpCharacterProxy*>(refObject);
				if (hkpObject) {
					// This one appears to be active during combat.
					// Maybe used for sword swing collision detection
					for (auto phantom: hkpObject->phantoms) {
						this->AddPhantom(phantom);
					}

					// This is the actual shape
					if (hkpObject->shapePhantom) {
						this->AddPhantom(hkpObject->shapePhantom);
					}
				}
			}
		}
		else if (charRigidBodyController) {
			// NPCs seem to use rigid body ones
			auto& characterRigidBody = charRigidBodyController->characterRigidBody;
			hkReferencedObject* refObject = characterRigidBody.referencedObject.get();
			if (refObject) {
				hkpCharacterRigidBody* hkpObject = skyrim_cast<hkpCharacterRigidBody*>(refObject);
				if (hkpObject) {
					if (hkpObject->m_character) {
						this->AddRB(hkpObject->m_character);
					}
				}
			}
		}
	}
}
