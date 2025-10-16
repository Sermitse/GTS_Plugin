#pragma once

#ifdef GTS_PROFILER_ENABLED

namespace GTS {

    class EntrypointProfilerHandle {
        public:
        EntrypointProfilerHandle(std::string_view name);
        ~EntrypointProfilerHandle();
        private:
        std::string Name;
    };

}

#endif //GTS_PROFILER_ENABLED