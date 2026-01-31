#pragma once

namespace GTS {

	class LaunchActor {
		public:
		static void ApplyLaunchTo(Actor* giant, Actor* tiny, float force, float launch_power);
		static void ApplyLaunch_At(Actor* giant, float radius, float power, FootEvent kind);
		static void LaunchAtNode(Actor* giant, float radius, float power, std::string_view node);
		static void LaunchAtNode(Actor* giant, float radius, float power, NiAVObject* node);
		static void LaunchAtCustomNode(Actor* giant, float radius, float min_radius, float power, NiAVObject* node);
		static void LaunchAtFoot(Actor* giant, float radius, float power, bool right_foot);
	};
}
