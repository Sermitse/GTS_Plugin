#pragma once
// Takes a actor and extracts its collision objects

#include "Colliders/ColliderData.hpp"
#include "Colliders/CharacterController.hpp"
#include "Colliders/Ragdoll.hpp"

namespace GTS {

	class ActorCollisionData : public ColliderData {

		public:
		ActorCollisionData(Actor* actor);

		protected:
		virtual std::vector<ColliderData*> GetChildren() override;

		private:
		CharacterControllerData charCont;
		RagdollData ragdoll;

	};
}
