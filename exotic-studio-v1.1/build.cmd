@echo off
setlocal
cd /d "%~dp0"

echo.
echo ==========================================
echo   EXOTIC VX.0.6.0 WINDOWS BUILD
echo ==========================================
echo.

where cmake >nul 2>nul
if errorlevel 1 (
    echo ERROR: CMake was not found.
    echo Install Visual Studio 2022 with:
    echo - Desktop development with C++
    echo - CMake tools for Windows
    pause
    exit /b 1
)

powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0scripts\Build-Release.ps1"

if errorlevel 1 (
    echo.
    echo BUILD FAILED.
    pause
    exit /b 1
)

echo.
echo BUILD COMPLETE.
echo Output:
echo %~dp0dist\EXOTIC-VX.0.6.0\EXOTIC.exe
echo.
pause
endlocal
