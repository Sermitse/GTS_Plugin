#include "Debug/Profiler/Profiler.hpp"

#ifdef GTS_PROFILER_ENABLED

namespace GTS {


	Profiler::Profiler(std::string_view name) : Name(std::string(name)), ThreadId(std::this_thread::get_id()), LastAccessed(Clock::now()) {}

	void Profiler::Start() {
		if (!Running.load(std::memory_order_relaxed)) {
			ClkBegin = Clock::now();
			Running.store(true);
			UpdateLastAccessed();
		}
	}

	void Profiler::Stop() {
		if (Running.load(std::memory_order_relaxed)) {
			this->Elapsed += RunningTime();
			Running.store(false);
			UpdateLastAccessed();
		}
	}

	void Profiler::Reset() {
		this->Elapsed = 0.0f;
		UpdateLastAccessed();
	}

	double Profiler::GetElapsed() const {
		if (this->IsRunning()) {
			return this->Elapsed + this->RunningTime();
		}
		return this->Elapsed;
	}

	bool Profiler::IsRunning() const {
		return Running.load(std::memory_order_relaxed);
	}

	double Profiler::RunningTime() const {
		if (Running.load(std::memory_order_relaxed)) {
			return std::chrono::duration_cast<Second>(Clock::now() - ClkBegin).count();
		}
		return 0;
	}

	std::string Profiler::GetName() {
		return this->Name;
	}

	std::thread::id Profiler::GetThreadId() const {
		return this->ThreadId;
	}

	void Profiler::UpdateLastAccessed() {
		LastAccessed = Clock::now();
	}

	bool Profiler::IsExpired(double expiration_time_seconds) const {
		auto now = Clock::now();
		auto elapsed = std::chrono::duration_cast<Second>(now - LastAccessed).count();
		return elapsed > expiration_time_seconds;
	}

}

#endif //GTS_PROFILER_ENABLED