[![CI](https://github.com/Sermitse/GTS_Plugin/actions/workflows/build.yml/badge.svg)](https://github.com/Sermitse/GTS_Plugin/actions/workflows/build.yml)

# Size Matters SKSE64 Plugin

This is the source for the [Gianttess Mod - Size matters NG](https://www.nexusmods.com/skyrimspecialedition/mods/37823?tab=description) SKSE plugin.

## Building requirements

Before building, ensure the following tools are installed and properly configured:

- [**Git**](https://gitforwindows.org/) — add git to your system PATH.
- [**CMake**](https://cmake.org/download/) — add cmake to your system PATH.
- [**vcpkg**](https://github.com/microsoft/vcpkg) — set the VCPKG_ROOT environment variable to your vcpkg installation directory.
- [**Visual Studio 2022 (MSVC 14.39) or newer**](https://visualstudio.microsoft.com/vs/) — install with the Desktop development with C++ workload.
- [**Ninja**](https://ninja-build.org/) — should be bundled with the C++ workload for VS but can alternatively be installed through winget with:
```
winget install --id=Ninja-build.Ninja -e
```

## Building the project

This is a standard ``CMake with vcpgkg``-based project. Once all requirements are met, you can build in one of two ways:

### Option 1 — Visual Studio 
1. Clone the repository using Git.
2. Open the cloned folder in Visual Studio.
3. Visual Studio if setup correctly should automatically detect the `CMakeLists.txt` file and configure the build environment.

### Option 2 — Automated (Recommended)
Simply run the provided batch file:
```
_CompileDLL.bat
```
If successfull a `Package-Release` folder should have been created within the `distribution` folder. Containing the built DLL along with other runtime requirements.
> **Note:**  
> You must clone the repository using Git.  
> Direct downloads from GitHub (ZIP files) will **not** compile due to missing git data which the build process depends on.


## Modder API
The plugin exposes a few basic functions through both a papyrus and native interface.

### Papyrus
As long as you have the source psc's you can simply call the exported functions in your script (eg. `float scale = GTSScale.GetVisualScale(akActor)`) .

A full list of exported functions can be found in `distribution\PapyrusSource\`

### Native API
A native API for inter DLL communication also exists. To use it simply copy `GTSPluginAPI.hpp`
located in `src/API/External/` file into your project and call the RequestPluginAPI function
in either `kPostPostLoad` or `kDataLoaded` SKSE message events.


## Current feature wishlist
- [ ] [1] Automatic blood decal placement on the Player/NPC after doing certain actions.

## Implemented features that need improving
- [X] [1] Affect detection based on scale (Partially done)
- [X] [2] NPC Movement state correction (Limit NPC Movement to just walk and jog states). Partially works and only on the AI Follow procedure.