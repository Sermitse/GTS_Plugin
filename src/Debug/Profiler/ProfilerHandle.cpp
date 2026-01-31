#include "Debug/Profiler/ProfilerHandle.hpp"
#include "Debug/Profilers.hpp"

#ifdef GTS_PROFILER_ENABLED

namespace GTS {

	ProfilerHandle::ProfilerHandle(std::string_view name) : Name(std::string(name)) {
		Profilers::Start(name);
	}

	ProfilerHandle::~ProfilerHandle() {
		Profilers::Stop(this->Name);
	}
}

#endif //GTS_PROFILER_ENABLED