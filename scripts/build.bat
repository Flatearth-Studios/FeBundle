@echo off
setlocal

:: ========================
:: Configuration
:: ========================
set BUILD_DIR=build
set BUILD_TYPE=Debug

:: Clean build dir if exists
if exist %BUILD_DIR% (
    echo [Clean] Removing old build dir...
    rmdir /s /q %BUILD_DIR%
)

mkdir %BUILD_DIR%
cd %BUILD_DIR%

:: ========================
:: Step 1: Run the exact working CMake command
:: ========================
echo [CMake] Configuring project...
cmake -S .. -B . -G "Visual Studio 17 2022" -A x64 ^
  -DFEBUNDLE_COMPILED=ON ^
  -DCMAKE_TOOLCHAIN_FILE="C:\vcpkg\scripts\buildsystems\vcpkg.cmake" ^
  -DVCPKG_TARGET_TRIPLET=x64-windows ^
  -DCMAKE_BUILD_TYPE=%BUILD_TYPE%

if %errorlevel% neq 0 (
    echo [ERROR] CMake configure failed!
    exit /b %errorlevel%
)

:: ========================
:: Step 2: Build the solution
:: ========================
echo [CMake] Building %BUILD_TYPE%...
cmake --build . --config %BUILD_TYPE%

if %errorlevel% neq 0 (
    echo [ERROR] Build failed!
    exit /b %errorlevel%
)

:: ========================
:: Done
:: ========================
echo [SUCCESS] FeBundle built successfully.
cd ..
endlocal
