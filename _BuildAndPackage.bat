echo.
echo Running _CompileDLL.bat...
call _CompileDLL.bat
if %errorlevel% neq 0 (
    echo _CompileDLL.bat failed.
    exit /b 1
)

echo.
echo Running _CreateModPackage.bat...
call _CreateModPackage.bat
if %errorlevel% neq 0 (
    echo _CreateModPackage.bat failed.
    exit /b 1
)
