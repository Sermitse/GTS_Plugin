#pragma once
#include "Debug/Profiler/EntrypointProfilerHandle.hpp"
#include "Debug/Profiler/Profiler.hpp"
#include "Debug/Profiler/ProfilerHandle.hpp"
#include "Utils/Misc/Singleton.hpp"

#ifdef GTS_PROFILER_ENABLED

#define GTS_PROFILE_ENTRYPOINT_UNIQUE(name, ID)                      \
    auto _gts_profile_handle = GTS::Profilers::ProfileEntrypoint(    \
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

namespace GTS {

    class Profilers : CInitSingleton<Profilers> {

		private:

        struct DisplaySnapshot {

            struct ThreadSnapshot {
                tbb::concurrent_vector<std::pair<std::string, double>> profilers;
                tbb::concurrent_vector<std::pair<std::string, double>> entrypoint_profilers;
                std::chrono::time_point<std::chrono::steady_clock> last_activity;
            };

            tbb::concurrent_unordered_map<std::thread::id, ThreadSnapshot> threads;
            double total_time = 0.0;
            bool is_valid = false;
        };

        struct ThreadProfilerData {

            tbb::concurrent_unordered_map<std::string, Profiler> profilers;
            tbb::concurrent_unordered_map<std::string, Profiler> entrypoint_profilers;
            std::chrono::time_point<std::chrono::steady_clock> last_activity;

            ThreadProfilerData() : last_activity(std::chrono::steady_clock::now()) {}

        };

        static bool AnyRunning();
        static bool AnyEntrypointRunning();
        static void CleanupExpiredThreads();
        static std::string GetThreadName(std::thread::id thread_id);
        static void DisplayThreadTable(const std::string& thread_name, ThreadProfilerData& data, double total_time);
        static void DisplayThreadTableFromSnapshot(const std::string& thread_name, std::thread::id tid, const DisplaySnapshot::ThreadSnapshot& data, double total_time);

        static inline tbb::concurrent_unordered_map<std::thread::id, ThreadProfilerData> thread_data;
        static inline tbb::concurrent_unordered_map<std::thread::id, std::string> thread_names;
        static inline std::chrono::time_point<std::chrono::steady_clock> last_display_update = std::chrono::steady_clock::now();
        static inline std::mutex snapshot_mutex;

        static inline DisplaySnapshot display_snapshot;
        static inline Profiler TotalTime = Profiler("TotalTime"sv);


        public:

        [[nodiscard]] static ProfilerHandle Profile(std::string_view name);
	    [[nodiscard]] static EntrypointProfilerHandle ProfileEntrypoint(std::string_view name);

	    static void Start(std::string_view a_name);
        static void Stop(std::string_view a_name);
        static void StartEntrypoint(std::string_view a_name);
        static void StopEntrypoint(std::string_view a_name);
        static void DisplayReport();
        static void SetThreadExpirationTime(double seconds);
	    static double GetThreadExpirationTime();

        static inline double thread_expiration_time = 30.0;
        static inline double update_interval = 0.5;
        static inline bool DrawProfiler = false;

    };
}

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

#endif //GTS_PROFILER_ENABLED