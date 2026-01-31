#pragma once
// Takes a actor and extracts its collision objects

#include "Systems/Colliders/ColliderData.hpp"
#include "Systems/Colliders/CharacterController.hpp"
#include "Systems/Colliders/Ragdoll.hpp"

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
