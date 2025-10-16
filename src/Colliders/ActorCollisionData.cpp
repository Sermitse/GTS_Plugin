#include "Colliders/ActorCollisionData.hpp"

namespace GTS {
	
	ActorCollisionData::ActorCollisionData(Actor* actor) : charCont(actor ? actor->GetCharController() : nullptr), ragdoll(actor ? GetRagdoll(actor) : nullptr) {}

	std::vector<ColliderData*> ActorCollisionData::GetChildren() {
		return {
	        &this->charCont,
	        &this->ragdoll
		};
	}
}
