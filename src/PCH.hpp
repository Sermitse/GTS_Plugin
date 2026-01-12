#pragma once

#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <barrier>
#include <bit>
#include <bitset>
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfenv>
#include <cfloat>
#include <charconv>
#include <chrono>
#include <cinttypes>
#include <climits>
#include <clocale>
#include <cmath>
#include <compare>
#include <complex>
#include <concepts>
#include <condition_variable>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cuchar>
#include <cwchar>
#include <cwctype>
#include <d3d11.h>
#include <deque>
#include <ehdata.h>
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
#include <ios>
#include <iosfwd>
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
#include <ratio>
#include <rttidata.h>
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
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <variant>
#include <vector>
#include <version>
#include <wincodec.h>
#include <wrl/client.h>

#include <RE/Skyrim.h>
#include <REL/Relocation.h>
#include <SKSE/SKSE.h>

#include <Psapi.h>
#include <ShlObj_core.h>
#include <tchar.h>
#include <Windows.h>

// For console sink
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <lunasvg.h>					  //https://github.com/sammycage/lunasvg
#include <reflect>                        //https://github.com/qlibs/reflect
#include <toml.hpp>                       //https://github.com/ToruNiina/toml11
#include <magic_enum/magic_enum.hpp>      //https://github.com/Neargye/magic_enum
#include <lz4.h>						  //https://github.com/lz4/lz4
#include <glm/ext.hpp>					  //https://github.com/g-truc/glm
#include <glm/glm.hpp>
#include <detours/detours.h>			  //https://github.com/microsoft/Detours
#include <glaze/json/read.hpp>            //https://github.com/stephenberry/glaze
#include <absl/container/flat_hash_map.h> //https://github.com/abseil/abseil-cpp
#include <re2/re2.h>                      //https://github.com/google/re2

//Imgui Defines
#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#define IMGUI_DEFINE_MATH_OPERATORS

//Imgui
#include "UI/Lib/imgui.h"
#include "UI/Lib/imgui_internal.h"
#include "UI/Lib/imgui_stdlib.h"
#include "UI/Lib/imgui_impl_dx11.h"

//Tbb - https://github.com/uxlfoundation/oneTBB
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_map.h>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/parallel_for_each.h>
#include <tbb/concurrent_queue.h>

#pragma hdrstop  // Everything after this is NOT precompiled

using namespace std::literals;
using namespace REL::literals;

namespace GTS {
	using namespace std;
	using namespace SKSE;
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

namespace logger = SKSE::log;

//Project-Wide Defines
//#define GTS_PROFILER_ENABLED
//#define GTS_DISABLE_PLUGIN

//WinAPI Fix
#undef PlaySound 
#undef DeleteFile
#undef LoadImage

// ---- Own Includes ----
#include "Constants.hpp"

//Debugging
#include "Debug/Profilers.hpp"
#include "Debug/DebugDraw.hpp"

//Base Utils
#include "Utils/Misc/Singleton.hpp"
#include "Utils/Misc/Enum.hpp"
#include "Utils/Misc/Random.hpp"
#include "Utils/Misc/TryLockMutex.hpp"
#include "Utils/Text/Text.hpp"
#include "Utils/Text/Format.hpp"
#include "Utils/Input/DInput.hpp"
#include "Utils/Win32/Messagebox.hpp"

//Systems
#include "Systems/Events/EventListener.hpp"
#include "Systems/Events/EventDispatcher.hpp"
#include "Systems/Misc/State.hpp"
#include "Systems/Misc/Tasks.hpp"
#include "Systems/Misc/Time.hpp"
#include "Systems/Misc/Timer.hpp"
#include "Systems/Motion/Spring.hpp"
#include "Systems/Motion/Smooth.hpp"

//GTS Utils
#include "Utils/PapyrusUtils.hpp"
#include "Utils/Units.hpp"
#include "Utils/Camera.hpp"
#include "Utils/Node.hpp"
#include "Utils/EffectUtils.hpp"
#include "Utils/QuestUtil.hpp"
#include "Utils/SoundUtils.hpp"

//Scale
#include "Scale/Scale.hpp"
#include "Scale/ModScale.hpp"
#include "Scale/Height.hpp"
#include "Scale/ScaleUtils.hpp"

//Other
#include "Papyrus/ProxyFunctions.hpp"

//Actor
#include "Utils/Actions/ActionUtils.hpp"
#include "Utils/Actor/ActorUtils.hpp"
#include "Utils/Actor/ActorBools.hpp"
#include "Utils/Actor/GTSUtils.hpp"
#include "Utils/Actor/SkillUtils.hpp"
#include "Utils/Actor/FindActor.hpp"
#include "Utils/Actor/AV.hpp"
#include "Utils/Animation/AnimationVars.hpp"

//Data
#include "Data/Runtime.hpp"
#include "Data/Persistent.hpp"
#include "Data/Transient.hpp"
