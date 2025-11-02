#pragma once

namespace DebugUtil {

	class ObjectBound {

		public:
		glm::vec3 boundMin = {};
		glm::vec3 boundMax = {};
		glm::vec3 worldBoundMin = {};
		glm::vec3 worldBoundMax = {};
		glm::vec3 rotation = {};

		ObjectBound();
		ObjectBound(glm::vec3 pBoundMin, glm::vec3 pBoundMax, glm::vec3 pWorldBoundMin, glm::vec3 pWorldBoundMax, glm::vec3 pRotation);
		glm::vec3 GetBoundRightVectorRotated() const;
		glm::vec3 GetBoundForwardVectorRotated() const;
		glm::vec3 GetBoundUpVectorRotated() const;

	};
}