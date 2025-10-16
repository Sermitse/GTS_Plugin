#pragma once

#ifdef GTS_PROFILER_ENABLED

namespace GTS {

    class Profiler {
        private:
        using Clock = std::chrono::steady_clock;
        using Second = std::chrono::duration<double>;

        std::atomic<double> Elapsed{ 0.0 };
        std::atomic<bool> Running{ false };
        std::string Name = "";
        std::thread::id ThreadId;
        std::chrono::time_point<Clock> LastAccessed;
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

}

#endif //GTS_PROFILER_ENABLED