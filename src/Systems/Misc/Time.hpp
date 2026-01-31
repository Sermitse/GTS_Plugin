#pragma once

namespace GTS {

	class Time {

		public:
		static float WorldTimeDelta();
		static double WorldTimeElapsed();
		static std::uint64_t FramesElapsed();
		static float GGTM();
		static void SGTM(float modifier);
		static void Update();

		private:
		inline static double worldTimeElapsed = 0.0;
		inline static std::uint64_t framesElapsed = 0;
	};
}
