@echo off
setlocal

echo Checking dependencies...

:: Locate Visual Studio using vswhere
set VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq delims=" %%i in (`%VSWHERE% -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_PATH=%%i"

if not defined VS_PATH (
    echo Visual Studio with MSVC tools not found.
    echo Ensure "Desktop development with C++" is installed.
    exit /b 1
)

echo Found Visual Studio at "%VS_PATH%"
call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"

:: Check CMake
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo CMake not found. Please install it or add it to PATH.
    exit /b 1
) else (
    echo CMake found.
)

:: Check Ninja
where ninja >nul 2>&1
if %errorlevel% neq 0 (
    echo Ninja not found. Please install it or add it to PATH.
    exit /b 1
) else (
    echo Ninja found.
)

:: Check vcpkg via known environment variable
if defined VCPKG_ROOT (
    set "VCPKG_PATH=%VCPKG_ROOT%\vcpkg.exe"
) else if defined VCPKG_INSTALLATION_ROOT (
    set "VCPKG_PATH=%VCPKG_INSTALLATION_ROOT%\vcpkg.exe"
) else (
    set "VCPKG_PATH="
)

if not exist "%VCPKG_PATH%" (
    echo vcpkg not found. Please install it or define VCPKG_ROOT or VCPKG_INSTALLATION_ROOT.
    exit /b 1
) else (
    echo vcpkg found at "%VCPKG_PATH%"
)

echo.
echo Setting up build environment...
if not defined VS_PATH (
    echo Visual Studio environment already configured.
) else (
    call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat"
)

echo.
echo Running CMake configuration...
cmake --preset config-release
if %errorlevel% neq 0 (
    echo CMake configuration failed.
    exit /b 1
)

echo.
echo Building project...
cmake --build --preset build-release
if %errorlevel% neq 0 (
    echo Build failed.
    exit /b 1
)

echo.
echo Done.
endlocal
exit /b 0
