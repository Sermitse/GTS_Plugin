#pragma once

#ifdef GTS_PROFILER_ENABLED
#define GTS_PROFILE_ENTRYPOINT_UNIQUE(name, ID) \
    auto _gts_profile_handle = GTS::Profilers::ProfileEntrypoint( \
        std::string(name) + "<" + std::to_string(ID) + ">" )
#define GTS_PROFILE_ENTRYPOINT(name) auto _gts_profile_handle = GTS::Profilers::ProfileEntrypoint(name)
#define GTS_PROFILE_SCOPE(name) auto _gts_profile_handle = GTS::Profilers::Profile(name)
#define GTS_PROFILE_ENTRYPOINT_STATIC(name) thread_local static auto _gts_profile_handle = GTS::Profilers::ProfileEntrypoint(name)
#define GTS_PROFILE_SCOPE_STATIC(name) thread_local static auto _gts_profile_handle = GTS::Profilers::Profile(name)
#define GTS_PROFILER_START(name) GTS::Profilers::Start(name)
#define GTS_PROFILER_STOP(name) GTS::Profilers::Stop(name)
#define GTS_PROFILER_START_ENTRYPOINT(name) GTS::Profilers::StartEntrypoint(name)
#define GTS_PROFILER_STOP_ENTRYPOINT(name) GTS::Profilers::StopEntrypoint(name)
#define GTS_PROFILER_DISPLAY_REPORT() GTS::Profilers::DisplayReport()
#else

#define GTS_PROFILE_ENTRYPOINT_UNIQUE(name, ID)
#define GTS_PROFILE_ENTRYPOINT(name)
#define GTS_PROFILE_SCOPE(name)
#define GTS_PROFILE_ENTRYPOINT_STATIC(name)
#define GTS_PROFILE_SCOPE_STATIC(name)
#define GTS_PROFILER_START(name)
#define GTS_PROFILER_STOP(name)
#define GTS_PROFILER_START_ENTRYPOINT(name)
#define GTS_PROFILER_STOP_ENTRYPOINT(name)
#define GTS_PROFILER_DISPLAY_REPORT()
#endif

namespace GTS {

	class Profiler {

		private:
		using Clock = std::chrono::steady_clock;
		using Second = std::chrono::duration<double, std::ratio<1>>;

		std::chrono::time_point<Clock> ClkBegin{
			Clock::now()
		};

		double Elapsed = 0.0;
		bool Running = false;
		std::string Name = "";
		std::thread::id ThreadId;
		std::chrono::time_point<Clock> LastAccessed;


		public:
		explicit Profiler(std::string_view name);
		void Start();
		void Stop();
		void Reset();
		double GetElapsed() const;
		bool IsRunning() const;
		double RunningTime() const;
		std::string GetName();
		std::thread::id GetThreadId() const;
		void UpdateLastAccessed();
		bool IsExpired(double expiration_time_seconds) const;
	};

	class ProfilerHandle {
		public:
		ProfilerHandle(std::string_view name);
		~ProfilerHandle();
		private:
		std::string Name;
	};

	class EntrypointProfilerHandle {
		public:
		EntrypointProfilerHandle(std::string_view name);
		~EntrypointProfilerHandle();
		private:
		std::string Name;
	};

	struct ThreadProfilerData {
		std::unordered_map<std::string, Profiler> profilers;
		std::unordered_map<std::string, Profiler> entrypoint_profilers;
		std::chrono::time_point<std::chrono::steady_clock> last_activity;

		ThreadProfilerData() : last_activity(std::chrono::steady_clock::now()) {}
	};

	class Profilers {
		public:
		[[nodiscard]] static ProfilerHandle Profile(std::string_view name) {
			return {
				name
			};
		}

		[[nodiscard]] static EntrypointProfilerHandle ProfileEntrypoint(std::string_view name) {
			return {
				name
			};
		}

		static void Start(std::string_view a_name);
		static void Stop(std::string_view a_name);
		static void StartEntrypoint(std::string_view a_name);
		static void StopEntrypoint(std::string_view a_name);
		static void DisplayReport();

		// Thread cleanup configuration
		static void SetThreadExpirationTime(double seconds) {
			thread_expiration_time = seconds;
		}
		static double GetThreadExpirationTime() {
			return thread_expiration_time;
		}

		static inline bool DrawProfiler = false;

		private:
		bool AnyRunning();
		bool AnyEntrypointRunning();
		void CleanupExpiredThreads();
		static std::string GetThreadName(std::thread::id thread_id);
		static void DisplayThreadTable(const std::string& thread_name, ThreadProfilerData& data, double total_time);

		[[nodiscard]] static Profilers& GetSingleton();

		std::unordered_map<std::thread::id, ThreadProfilerData> thread_data;
		Profiler TotalTime = Profiler("TotalTime"sv);

		static inline double thread_expiration_time = 30.0; // Default 30 seconds
		static inline std::unordered_map<std::thread::id, std::string> thread_names;
		static inline std::mutex thread_names_mutex;
		
	};
}