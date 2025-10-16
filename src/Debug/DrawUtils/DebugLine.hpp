#pragma once

#include <glm/glm.hpp>

namespace DebugUtil {

	class DebugLine {

		public:
		DebugLine(glm::vec3 from, glm::vec3 to, glm::vec4 color, float lineThickness, unsigned __int64 destroyTickCount);

		glm::vec3 From;
		glm::vec3 To;
		glm::vec4 Color;
		float fColor;
		float Alpha;
		float LineThickness;
		unsigned __int64 DestroyTickCount;
	};
}
