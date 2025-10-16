#pragma once
// Module that holds data that is loaded at runtime
// This includes various forms

namespace GTS {

	class Time {

		public:
		static float WorldTimeDelta();
		static double WorldTimeElapsed();
		static std::uint64_t FramesElapsed();
		static float GGTM();
		static void SGTM(float modifier);
		[[nodiscard]] static Time& GetSingleton() noexcept;
		void Update();

		private:
		double worldTimeElapsed = 0.0;
		std::uint64_t framesElapsed = 0;
	};
}
