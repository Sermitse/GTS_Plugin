#include "Systems/Misc/Time.hpp"

namespace {
	inline float* g_delta_time = reinterpret_cast<float*>(REL::RelocationID(523660, 410199, NULL).address());
	inline float* g_SGTM = reinterpret_cast<float*>(REL::RelocationID(511883, 388443, NULL).address());
}

namespace GTS {

	float Time::WorldTimeDelta() {
		return (*g_delta_time);
	}

	double Time::WorldTimeElapsed() {
		return worldTimeElapsed;
	}

	std::uint64_t Time::FramesElapsed() {
		return framesElapsed;
	}

	float Time::GGTM() {
		return (*g_SGTM);
	}

	void Time::SGTM(float modifier) {
		*g_SGTM = modifier;
		using func_t = decltype(SGTM);
		REL::Relocation<func_t> func{ RELOCATION_ID(66989, 68246) };
		func(modifier);
	}

	void Time::Update() {
		//log::info("FramesElapsed: {}, WorldTimeElapsed: {}", this->framesElapsed, Time::WorldTimeDelta());
		framesElapsed += 1;
		worldTimeElapsed += WorldTimeDelta();
	}
}
