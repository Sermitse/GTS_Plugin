cmake_minimum_required(VERSION 3.21)
cmake_policy(VERSION 3.21...3.31)

# #######################################################################################################################
# # Project Defenition
# #######################################################################################################################

project(
	GtsPlugin
	VERSION 3.5.0.3
	DESCRIPTION "The main SKSE plugin of the Size Matters mod"
	LANGUAGES CXX
)

# Not Explosed By Cmake, Manually Set Here for version.rc.in
set(PROJECT_COPYRIGHT "Apache License 2.0")
set(PROJECT_DESCRIPTION "${PROJECT_DESCRIPTION}")
set(PROJECT_NAME "${PROJECT_NAME}")
set(PROJECT_FRIENDLY_NAME "${PROJECT_NAME}")
#set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Options
option(GTS_STRICT_COMPILE "Enable strict compilation flags (treat warnings as errors)" OFF)
option(GTS_DEPLOY_TO_FOLDER "Copy The Built DLL and PDB to the EnvVar Defined Target location" ON)
option(GTS_BUILD_DISTRIBUTION "Construct a distribution folder after each build" ON)

# #######################################################################################################################
# # Flags
# #######################################################################################################################

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(_DISABLE_EXTENDED_ALIGNED_STORAGE)

# Check if MSVC is the compiler
if(MSVC)
    # Extract the version number of the MSVC compiler
    string(REGEX REPLACE "^([0-9]+)\\.([0-9]+).*" "\\1.\\2" MSVC_VERSION_STRING "${MSVC_VERSION}")
    message(STATUS "Detected MSVC version: ${MSVC_VERSION_STRING}")

    # Check if the MSVC version is at least 14.39
    if(MSVC_VERSION LESS 1939)
        message(FATAL_ERROR "MSVC version 14.39 or higher is required. Detected version: ${MSVC_VERSION_STRING}")
    endif()
else()
    message(FATAL_ERROR "This project currently only supports MSVC as the compiler.")
endif()

# #######################################################################################################################
# # Project modules
# #######################################################################################################################
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(xSEPlugin)

# #######################################################################################################################
# # Find dependencies - VCPKG Ports
# #######################################################################################################################

find_path(CLIB_UTIL_INCLUDE_DIRS "ClibUtil/Utils.hpp")

# #######################################################################################################################
# # Find dependencies - VCPKG Other
# #######################################################################################################################

find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
find_library(DETOURS_LIBRARY detours REQUIRED)

# #######################################################################################################################
# # Find dependencies - Git
# #######################################################################################################################

include(FetchContent)

# Git Version Tracking
FetchContent_Declare(
	cmake_git_version_tracking                   
    GIT_REPOSITORY https://github.com/andrew-hardin/cmake-git-version-tracking.git
    GIT_TAG 6c0cb87edd029ddfb403a8e24577c144a03605a6
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(cmake_git_version_tracking)

# Boost Reflect
# https://github.com/qlibs/reflect
FetchContent_Declare(
    boost_ext_reflect
    GIT_REPOSITORY https://github.com/BingusEx/reflect/
    GIT_SHALLOW ON
    GIT_TAG 653c94092fa8702dbc33ebf3b570190fb1a9dc28
)
FetchContent_MakeAvailable(boost_ext_reflect)

FetchContent_Declare(
  glaze
  GIT_REPOSITORY https://github.com/stephenberry/glaze.git
  GIT_TAG v7.0.0
  GIT_SHALLOW ON
)
FetchContent_MakeAvailable(glaze)

# #######################################################################################################################
# # Find dependencies - VCPKG
# #######################################################################################################################

find_package(Boost REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(lz4 CONFIG REQUIRED)
find_package(xbyak CONFIG REQUIRED)
find_package(toml11 CONFIG REQUIRED)
find_package(lunasvg CONFIG REQUIRED)
find_package(tbb CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(re2 CONFIG REQUIRED)

# #######################################################################################################################
# # Non-CLib Includes
# #######################################################################################################################

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
	${CLIB_UTIL_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS}
	${CLIBUTIL_INCLUDE_DIRS}
	${boost_ext_reflect_SOURCE_DIR}
    ${DETOURS_INCLUDE_DIRS}
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
    ${DETOURS_LIBRARY}
	xbyak::xbyak
	Boost::headers
	glaze::glaze
	toml11::toml11
	lz4::lz4
    lunasvg::lunasvg
    TBB::tbb
	absl::base
	absl::flat_hash_map
	re2::re2
	cmake_git_version_tracking
)

# #######################################################################################################################
# # Packagging
# #######################################################################################################################
include(Package)

# ----- debug
message(STATUS "Using toolchain file:                   ${CMAKE_TOOLCHAIN_FILE}.")
message(STATUS "CMAKE_CXX_FLAGS:                        ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG:                  ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE:                ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO:         ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS:                 ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS_DEBUG:           ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS_RELEASE:         ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO:  ${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO}")

get_target_property(compile_flags GtsPlugin COMPILE_OPTIONS)
message(STATUS "GtsPlugin COMPILE_OPTIONS:      ${compile_flags}")

get_target_property(link_flags GtsPlugin LINK_OPTIONS)
message(STATUS "GtsPlugin LINK_OPTIONS:         ${link_flags}")