#include "Debug/Util/DebugLine.hpp"
#include "Debug/Util/DebugUtil.hpp"

namespace DebugUtil {

	DebugLine::DebugLine(glm::vec3 from, glm::vec3 to, glm::vec4 color, float lineThickness, unsigned __int64 destroyTickCount) {
		From = from;
		To = to;
		Color = color;
		fColor = RGBToHex(color);
		Alpha = color.a * 100.0f;
		LineThickness = lineThickness;
		DestroyTickCount = destroyTickCount;
	}

}