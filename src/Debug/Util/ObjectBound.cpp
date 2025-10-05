#include "Debug/Util/ObjectBound.hpp"
#include "Debug/Util/DebugUtil.hpp"

namespace DebugUtil {

	ObjectBound::ObjectBound() {
		boundMin = glm::vec3();
		boundMax = glm::vec3();
		worldBoundMin = glm::vec3();
		worldBoundMax = glm::vec3();
		rotation = glm::vec3();
	}

	ObjectBound::ObjectBound(glm::vec3 pBoundMin, glm::vec3 pBoundMax, glm::vec3 pWorldBoundMin, glm::vec3 pWorldBoundMax, glm::vec3 pRotation) {
		boundMin = pBoundMin;
		boundMax = pBoundMax;
		worldBoundMin = pWorldBoundMin;
		worldBoundMax = pWorldBoundMax;
		rotation = pRotation;
	}

	glm::vec3 ObjectBound::GetBoundRightVectorRotated() const {
		glm::vec3 bound(abs(boundMin.x - boundMax.x), 0.0f, 0.0f);
		auto boundRotated = DebugUtil::RotateVector(rotation, bound);
		return boundRotated;
	}

	glm::vec3 ObjectBound::GetBoundForwardVectorRotated() const {
		glm::vec3 bound(0.0f, abs(boundMin.y - boundMax.y), 0.0f);
		auto boundRotated = DebugUtil::RotateVector(rotation, bound);
		return boundRotated;
	}

	glm::vec3 ObjectBound::GetBoundUpVectorRotated() const {
		glm::vec3 bound(0.0f, 0.0f, abs(boundMin.z - boundMax.z));
		auto boundRotated = DebugUtil::RotateVector(rotation, bound);
		return boundRotated;
	}
}