#pragma once
// Takes a char controller and extracts its collision objects
#include "Systems/Colliders/ColliderData.hpp"

namespace GTS {

	class CharacterControllerData : public ColliderData {
		public:
		CharacterControllerData(bhkCharacterController* charCont);
	};
}
