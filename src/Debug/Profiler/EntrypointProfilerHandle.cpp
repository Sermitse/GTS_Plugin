#include "Debug/Profiler/EntrypointProfilerHandle.hpp"
#include "Debug/Profilers.hpp"

#ifdef GTS_PROFILER_ENABLED

namespace GTS {

	EntrypointProfilerHandle::EntrypointProfilerHandle(std::string_view name) : Name(std::string(name)) {
		Profilers::StartEntrypoint(name);
	}

	EntrypointProfilerHandle::~EntrypointProfilerHandle() {
		Profilers::StopEntrypoint(this->Name);
	}
}

#endif //GTS_PROFILER_ENABLED