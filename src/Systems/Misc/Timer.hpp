#pragma once

namespace GTS {

	class Timer {

		public:
		explicit Timer(double delta);
		bool ShouldRun();
		void UpdateDelta(float a_delta);
		double TimeDelta() const;
		bool ShouldRunFrame();
		std::uint64_t FrameDelta() const;
		void Reset();

		private:
		double delta = 0.01666;
		double last_time = 0.0;
		double elaped_time = 0.0;
		std::uint64_t last_frame = 0;
		std::uint64_t elaped_frame = 0;
	};
}
