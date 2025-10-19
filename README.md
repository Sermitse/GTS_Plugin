[![CI](https://github.com/Sermitse/GTS_Plugin/actions/workflows/build.yml/badge.svg)](https://github.com/Sermitse/GTS_Plugin/actions/workflows/build.yml)

# Size Matters SKSE64 Plugin

This is the source for the [Gianttess Mod - Size matters NG](https://www.nexusmods.com/skyrimspecialedition/mods/37823?tab=description) SKSE plugin.

## Building requirements

Before building, ensure the following tools are installed and properly configured:

- [**Git**](https://gitforwindows.org/) — add git to your system PATH.
- [**CMake**](https://cmake.org/download/) — add cmake to your system PATH.
- [**vcpkg**](https://github.com/microsoft/vcpkg) — set the VCPKG_ROOT environment variable to your vcpkg installation directory.
- [**Visual Studio 2022 or newer**](https://visualstudio.microsoft.com/vs/) — install with the Desktop development with C++ workload.
- [**Ninja**](https://ninja-build.org/) — can be installed via the Visual Studio Installer or with:
```
winget install --id=Ninja-build.Ninja -e
```

## Building the project

This is a standard **CMake**-based project. Once all requirements are met, you can build in one of two ways:

### Option 1 — Visual Studio 
1. Clone the repository using Git.
2. Open the cloned folder in Visual Studio.
3. Visual Studio will automatically detect the `CMakeLists.txt` file and configure the build environment.

### Option 2 — Command Line (Recommended)
Simply run the provided batch file:
```
CompileDLL.bat
```
If successfull a `Package-Release` folder should exist within the `distribution` folder.
> **Note:**  
> You must clone the repository using Git.  
> Direct downloads from GitHub (ZIP files) will **not** compile due to missing git data which the build process depends on.


## Current weature wishlist

- [ ] [1] Blood feet/hands after crushing.
- [ ] [2] Quest progression UI.
- [ ] [4] Racemenu morphs.
- [ ] [5] Improved code structure.
- [ ] [5] Improved code peformance.
- [ ] [6] Killfeed UI. 
- [ ] [7] Statuseffect/Active Buff Icons UI for the following mechanics:
- Life Absorption Stacks (Perk);
- Damage Reduction (passive);
- Size Reserve Stacks (buff);
- Aspect Of Giantess (buff);
- Vore Stacks (passive);
- On The Edge (Perk);

## Implemented features that need improving
- [X] [1] Correct Collider Scaling (Currently done through Side-Mod)
- [X] [2] Affect detection based on scale (Partially done)



