#pragma once

#ifdef GTS_PROFILER_ENABLED

namespace GTS {

    class ProfilerHandle {
        public:
        ProfilerHandle(std::string_view name);
        ~ProfilerHandle();
        private:
        std::string Name;
    };

}

#endif //GTS_PROFILER_ENABLED