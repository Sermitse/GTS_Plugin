#include "Systems/Time.hpp"

namespace {
	inline static float* g_delta_time = (float*)REL::RelocationID(523660, 410199).address();
	inline static float* g_SGTM = (float*)REL::RelocationID(511883, 388443).address();
}

namespace GTS {

	Time& Time::GetSingleton() noexcept {
		static Time instance;
		return instance;
	}

	float Time::WorldTimeDelta() {
		return (*g_delta_time);
	}

	double Time::WorldTimeElapsed() {
		return Time::GetSingleton().worldTimeElapsed;
	}

	std::uint64_t Time::FramesElapsed() {
		return Time::GetSingleton().framesElapsed;
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
		this->framesElapsed += 1;
		this->worldTimeElapsed += Time::WorldTimeDelta();
	}
}
