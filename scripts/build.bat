y@echo off
setlocal

:: ========================
:: configuration
:: ========================
set build_dir=build
set build_type=debug

:: clean build dir if exists
if exist %build_dir% (
    echo [clean] removing old build dir...
    rmdir /s /q %build_dir%
)

mkdir %build_dir%
cd %build_dir%

:: ========================
:: step 1: run the exact working cmake command
:: ========================
echo [cmake] configuring project...
cmake -s .. -b . -g "visual studio 17 2022" -a x64 ^
  -dfebundle_compiled=on ^
  -dcmake_toolchain_file="c:\vcpkg\scripts\buildsystems\vcpkg.cmake" ^
  -dvcpkg_target_triplet=x64-windows ^
  -dcmake_build_type=%build_type%

if %errorlevel% neq 0 (
    echo [error] cmake configure failed!
    exit /b %errorlevel%
)

:: ========================
:: step 2: build the solution
:: ========================
echo [cmake] building %build_type%...
cmake --build . --config %build_type%

if %errorlevel% neq 0 (
    echo [error] build failed!
    exit /b %errorlevel%
)

xcopy ".\assets" ".\build\testbed\Release\assets" /E /I /Y
xcopy ".\assets" ".\build\testbed\Debug\assets" /E /I /Y

:: ========================
:: done
:: ========================
echo [success] febundle built successfully.
cd ..
endlocal
