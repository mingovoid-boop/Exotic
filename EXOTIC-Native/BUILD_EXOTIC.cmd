@echo off
setlocal
cd /d "%~dp0"
where cmake >nul 2>nul || (echo CMake is required. Install Visual Studio C++ and CMake. & pause & exit /b 1)
cmake -S . -B out\build -G "Visual Studio 17 2022" -A x64
if errorlevel 1 pause & exit /b 1
cmake --build out\build --config Release
if errorlevel 1 pause & exit /b 1
copy /Y "out\build\Release\EXOTIC.exe" "%USERPROFILE%\Desktop\EXOTIC.exe" >nul
start "" "%USERPROFILE%\Desktop\EXOTIC.exe"
exit /b 0
