@echo off
setlocal ENABLEDELAYEDEXECUTION

:: ---------------------------
:: FeBundle Visual Studio build
:: ---------------------------
:: Usage examples:
::   build_vs.bat
::   build_vs.bat Release x64 C:\SDKs\FeBundle on off
:: Args:
::   %1 = CONFIG        (Debug|Release|RelWithDebInfo|MinSizeRel) [default: Release]
::   %2 = ARCH          (x64|Win32|ARM64)                         [default: x64]
::   %3 = INSTALL_PREFIX (path)                                   [default: "C:\Program Files\FeBundle"]
::   %4 = FEBUNDLE_COMPILED (on|off)                              [default: off]
::   %5 = FEBUNDLE_BUILD_SHARED (on|off)                          [default: off]

set CFG=%~1
if "%CFG%"=="" set CFG=Release

set ARCH=%~2
if "%ARCH%"=="" set ARCH=x64

set PREFIX=%~3
if "%PREFIX%"=="" set PREFIX=C:\Program Files\FeBundle

set COMPILED=%~4
if /I "%COMPILED%"=="" set COMPILED=off

set SHARED=%~5
if /I "%SHARED%"=="" set SHARED=off

set GEN=Visual Studio 17 2022

echo.
echo ==== FeBundle - Configure (%GEN%, %ARCH%, %CFG%) ====
if exist build rmdir /S /Q build
mkdir build || goto :error

cmake -S . -B build -G "%GEN%" -A %ARCH% ^
  -DCMAKE_CXX_STANDARD=23 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_CXX_EXTENSIONS=OFF ^
  -DFEBUNDLE_COMPILED=%COMPILED% ^
  -DFEBUNDLE_BUILD_SHARED=%SHARED% ^
  -DCMAKE_INSTALL_PREFIX="%PREFIX%"

IF ERRORLEVEL 1 goto :error

echo.
echo ==== Build ====
cmake --build build --config %CFG%
IF ERRORLEVEL 1 goto :error

echo.
echo ==== Install to %PREFIX% ====
cmake --install build --config %CFG%
IF ERRORLEVEL 1 goto :error

echo.
echo ✓ Done.
goto :eof

:error
echo.
echo ✗ Build failed (errorlevel %ERRORLEVEL%).
exit /b %ERRORLEVEL%
