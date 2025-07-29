#pragma once
#define _USE_MATH_DEFINES

//Project-Wide Defines
//#define GTS_PROFILER_ENABLED
//#define GTS_DISABLE_PLUGIN

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfenv>
#include <cfloat>
#include <cinttypes>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cuchar>
#include <cwchar>
#include <cwctype>
#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <barrier>
#include <bit>
#include <bitset>
#include <charconv>
#include <chrono>
#include <compare>
#include <complex>
#include <concepts>
#include <condition_variable>
#include <deque>
#include <exception>
#include <execution>
#include <filesystem>
#include <format>
#include <forward_list>
#include <fstream>
#include <functional>
#include <future>
#include <initializer_list>
#include <iomanip>
#include <iosfwd>
#include <ios>
#include <iostream>
#include <istream>
#include <iterator>
#include <latch>
#include <limits>
#include <locale>
#include <map>
#include <memory>
#include <memory_resource>
#include <mutex>
#include <new>
#include <numbers>
#include <numeric>
#include <optional>
#include <ostream>
#include <queue>
#include <random>
#include <ranges>
#include <regex>
#include <ratio>
#include <scoped_allocator>
#include <semaphore>
#include <set>
#include <shared_mutex>
#include <source_location>
#include <span>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <string_view>
#include <syncstream>
#include <system_error>
#include <thread>
#include <tuple>
#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <variant>
#include <vector>
#include <version>
#include <d3d11.h>
#include <ehdata.h>
#include <rttidata.h>

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>
#include <REL/Relocation.h>

#include <ShlObj_core.h>
#include <Windows.h>
#include <Psapi.h>
#include <tchar.h>

//WinAPI Fix
#undef PlaySound 
#undef DeleteFile

// For console sink

#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <reflect>                      //https://github.com/qlibs/reflect
#include <toml.hpp>                     //https://github.com/ToruNiina/toml11
#include <magic_enum/magic_enum.hpp>    //https://github.com/Neargye/magic_enum
#include <lz4.h>

using namespace std::literals;
using namespace REL::literals;

namespace reflect {
	//Fix ambiguity in reflects' usage of the detail namespace
	using namespace reflect::v1_2_5::detail;
}

namespace GTS {
	using namespace std;
	using namespace SKSE;
	using namespace RE;
}

namespace DebugUtil {
	using namespace GTS;
	using namespace RE;
}

namespace Hooks {
	using namespace std;
	using namespace SKSE;
	using namespace RE;
	using namespace RE::BSScript;
	using namespace RE::BSScript::Internal;
	using namespace GTS;
}

//Add Missing Game versions to our old version of clib-ng
namespace RE {
	constexpr REL::Version RUNTIME_SSE_1_6_659(1, 6, 659, 0);
	constexpr REL::Version RUNTIME_SSE_1_6_678(1, 6, 678, 0);
	constexpr REL::Version RUNTIME_SSE_1_6_1130(1, 6, 1130, 0);
	constexpr REL::Version RUNTIME_SSE_1_6_1170(1, 6, 1170, 0);
}

namespace logger = SKSE::log;

//git version tracking
#include "git.h"

//Own Includes
#include "Config/ConfigUtil.hpp"
#include "Constants.hpp"
#include "Profiler/Profiler.hpp"
#include "Events/Events.hpp"
#include "Utils/Utils.hpp"
#include "Scale/Scale.hpp"
#include "Scale/ModScale.hpp"
#include "Scale/Height.hpp"
#include "Data/Data.hpp"
#include "Hooks/Hooks.hpp"
#include "Version.hpp"
#include "RE/RE.hpp"