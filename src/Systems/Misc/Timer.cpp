#include "Systems/Misc/Timer.hpp"

namespace GTS {

	Timer::Timer(double delta) : delta(delta) {}

	bool Timer::ShouldRun() {

		if (this->delta == 0.0) return false;

		double currentTime = Time::WorldTimeElapsed();

		if (this->last_time + this->delta <= currentTime) {
			this->elaped_time = currentTime - this->last_time;
			std::uint64_t currentFrame = Time::FramesElapsed();
			this->elaped_frame = currentFrame - this->last_frame;
			this->Reset();
			return true;
		}
		return false;
	}

	void Timer::UpdateDelta(const float a_delta) {
		this->delta = a_delta;
	}

	double Timer::TimeDelta() const {
		return this->elaped_time;
	}

	bool Timer::ShouldRunFrame() {

		if (this->delta == 0.0) return false;

		std::uint64_t currentFrame = Time::FramesElapsed();

		if (Timer::ShouldRun()) {
			return true;
		}
		if (currentFrame == this->last_frame) {
			return true;
		}
		return false;
	}

	void Timer::Reset() {
		this->last_time = Time::WorldTimeElapsed();
		this->last_frame = Time::FramesElapsed();
	}

	std::uint64_t Timer::FrameDelta() const {
		return this->elaped_frame;
	}
}
