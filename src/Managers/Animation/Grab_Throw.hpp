#pragma once

namespace GTS {

	class Animation_GrabThrow {
		public:
		static void Throw_Actor(const ActorHandle& giantHandle, const ActorHandle& tinyHandle, NiPoint3 startCoords, NiPoint3 endCoords, std::string_view TaskName, float speedmult = 1.0f, float upDownAngle = 35.0f, float leftRightAngle = 0.0);
		static void RegisterEvents();
	};
}