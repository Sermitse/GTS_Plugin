#pragma once

namespace GTS {

	class Profiler {

		private:
			// Type aliases to make accessing nested type easier
			using Clock = std::chrono::steady_clock;
			using Second = std::chrono::duration<double, std::ratio<1>>;

			std::chrono::time_point<Clock> m_beg { Clock::now() };

			double elapsed = 0.0;

			std::string name = "";

			bool running = false;



		public:
			explicit Profiler(std::string_view name);
			void Start();
			void Stop();
			void Reset();
			double Elapsed() const;
			bool IsRunning() const;
			double RunningTime() const;
			std::string GetName();

			static inline bool ProfilerEnabled = false;
	};

	class ProfilerHandle {
		public:
			ProfilerHandle(std::string_view name);
			~ProfilerHandle();
		private:
			std::string name;
	};

	class Profilers {
		public:
			[[nodiscard]] static ProfilerHandle Profile(std::string_view name) {
				return { name };
			}

			static void Start(std::string_view name);
			static void Stop(std::string_view name);
			static void DisplayReport();

		private:
			bool AnyRunning();
			[[nodiscard]] static Profilers& GetSingleton();
			std::unordered_map<std::string,Profiler> profilers;
			Profiler totalTime = Profiler("TotalTime"sv);
	};
}
