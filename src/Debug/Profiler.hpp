#pragma once
#include <tbb/concurrent_unordered_map.h>

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

        std::atomic<double> Elapsed{ 0.0 };
        std::atomic<bool> Running{ false };
        std::string Name = "";
        std::thread::id ThreadId;
        std::chrono::time_point<Clock> LastAccessed;
        static inline double update_interval = 0.5;
        std::chrono::time_point<Clock> ClkBegin{ Clock::now() };


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
        tbb::concurrent_unordered_map<std::string, Profiler> profilers;
        tbb::concurrent_unordered_map<std::string, Profiler> entrypoint_profilers;
        std::chrono::time_point<std::chrono::steady_clock> last_activity;

        ThreadProfilerData() : last_activity(std::chrono::steady_clock::now()) {}
    };

    class Profilers : CInitSingleton<Profilers> {

        public:
        [[nodiscard]] static ProfilerHandle Profile(std::string_view name) {
            return { name };
        }

        [[nodiscard]] static EntrypointProfilerHandle ProfileEntrypoint(std::string_view name) {
            return { name };
        }

        static void Start(std::string_view a_name);
        static void Stop(std::string_view a_name);
        static void StartEntrypoint(std::string_view a_name);
        static void StopEntrypoint(std::string_view a_name);
        static void DisplayReport();

        static void SetThreadExpirationTime(double seconds) {
            thread_expiration_time = seconds;
        }

        static double GetThreadExpirationTime() {
            return thread_expiration_time;
        }


        // Snapshot data for display
        struct DisplaySnapshot {
            struct ThreadSnapshot {
                std::vector<std::pair<std::string, double>> profilers;
                std::vector<std::pair<std::string, double>> entrypoint_profilers;
                std::chrono::time_point<std::chrono::steady_clock> last_activity;
            };
            std::map<std::thread::id, ThreadSnapshot> threads;
            double total_time = 0.0;
            bool is_valid = false;
        };

        static inline double update_interval = 0.5;
        static inline bool DrawProfiler = false;
        static inline DisplaySnapshot display_snapshot;
        static inline std::mutex snapshot_mutex;

        private:
        bool AnyRunning();
        bool AnyEntrypointRunning();
        void CleanupExpiredThreads();
        static std::string GetThreadName(std::thread::id thread_id);
        static void DisplayThreadTable(const std::string& thread_name, ThreadProfilerData& data, double total_time);

        tbb::concurrent_unordered_map<std::thread::id, ThreadProfilerData> thread_data;
        Profiler TotalTime = Profiler("TotalTime"sv);
        static inline std::chrono::time_point<std::chrono::steady_clock> last_display_update = std::chrono::steady_clock::now();
        static inline double thread_expiration_time = 30.0;
        static inline tbb::concurrent_unordered_map<std::thread::id, std::string> thread_names;
        static void DisplayThreadTableFromSnapshot(const std::string& thread_name, std::thread::id tid, const DisplaySnapshot::ThreadSnapshot& data, double total_time);

    };
}