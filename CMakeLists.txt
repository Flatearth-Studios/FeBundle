cmake_minimum_required(VERSION 3.20)
project(FeBundle VERSION 1.0.0 LANGUAGES CXX)

# ---- Global C++23 + tooling ----
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- Options ----
option(FEBUNDLE_COMPILED "Build FeBundle as a compiled library (not header-only)" OFF)
option(FEBUNDLE_BUILD_SHARED "If compiled, build as shared (otherwise static)" OFF)
option(FEBUNDLE_CCDB "Generate a TU for compile_commands.json in header-only mode" ON)
option(FEBUNDLE_WITH_IMGUI "Enable Dear ImGui integration" ON)
option(FEBUNDLE_IMGUI_DEMO "Compile imgui_demo.cpp" OFF)
option(FEBUNDLE_IMGUI_SDL3_GL3 "Enable ImGui SDL3 + OpenGL3 backends" OFF)

# ---- Public headers ----
set(FEBUNDLE_HEADERS
    include/FeBundle/FeBundle.hpp
    include/FeBundle/Core/Logger.hpp
    include/FeBundle/Core/Defines.hpp
    include/FeBundle/Core/Memory/Memory.hpp
    include/FeBundle/Core/Window/Window.hpp
    include/FeBundle/Core/Renderer/Renderer.hpp
    include/FeBundle/Core/Clock.hpp
    include/FeBundle/Core/GameTypes.hpp
    include/FeBundle/Core/Entrypoint.hpp
    include/FeBundle/Core/Application.hpp
)

# ---- Sources (only used when compiled) ----
set(FEBUNDLE_SOURCES
    src/Core/Memory/Memory.cc
    src/Core/Window/Window.cc
    src/Core/Renderer/Renderer.cc
    src/Core/Clock.cc
    src/Core/Application.cc
)

if(FEBUNDLE_COMPILED)
  if(FEBUNDLE_BUILD_SHARED)
    add_library(fe_bundle SHARED ${FEBUNDLE_HEADERS} ${FEBUNDLE_SOURCES})
  else()
    add_library(fe_bundle STATIC ${FEBUNDLE_HEADERS} ${FEBUNDLE_SOURCES})
  endif()

  target_compile_features(fe_bundle PUBLIC cxx_std_23)
  target_include_directories(fe_bundle
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
  )
  # optional but handy if this static lib might be linked into a shared lib later
  set_target_properties(fe_bundle PROPERTIES POSITION_INDEPENDENT_CODE ON)

else()
  # Header-only
  add_library(fe_bundle INTERFACE)
  target_compile_features(fe_bundle INTERFACE cxx_std_23)
  target_include_directories(fe_bundle
    INTERFACE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
  )
endif()

if(FEBUNDLE_WITH_IMGUI)
  set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui)

  set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
  )
  if(FEBUNDLE_IMGUI_DEMO)
    list(APPEND IMGUI_SOURCES ${IMGUI_DIR}/imgui_demo.cpp)
  endif()

  set(IMGUI_BACKENDS)
  if(FEBUNDLE_IMGUI_SDL3_GL3)
    list(APPEND IMGUI_BACKENDS
      ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
      ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    )
  endif()

  add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_BACKENDS})
  target_compile_features(imgui PUBLIC cxx_std_23)
  target_include_directories(imgui PUBLIC
    $<BUILD_INTERFACE:${IMGUI_DIR}>
    $<BUILD_INTERFACE:${IMGUI_DIR}/backends>
    $<INSTALL_INTERFACE:include/FeBundle/3rdparty/imgui>
    $<INSTALL_INTERFACE:include/FeBundle/3rdparty/imgui/backends>
  )

  # Backend deps (optional)
  if(FEBUNDLE_IMGUI_SDL3_GL3)
    find_package(OpenGL QUIET)
    if(OpenGL_FOUND)
        target_link_libraries(imgui PUBLIC OpenGL::GL)
    elseif(WIN32)
        target_link_libraries(imgui PUBLIC opengl32)
    endif()
    find_package(SDL3 QUIET)  # Requires an SDL3 CMake package if present
    if(SDL3_FOUND)
        target_link_libraries(imgui PUBLIC SDL3::SDL3)
    else()
      message(STATUS "SDL3 not found; provide it in apps that use the SDL3 backend.")
    endif()
  endif()

  # Link ImGui to FeBundle
  if(TARGET fe_bundle)
    if(FEBUNDLE_COMPILED)
      # FeBundle uses ImGui in .cc files → PRIVATE (make PUBLIC if public headers expose <imgui.h>)
      target_link_libraries(fe_bundle PRIVATE imgui)
    else()
      # Header-only FeBundle: only INTERFACE if headers include <imgui.h>
      # target_link_libraries(fe_bundle INTERFACE imgui)
    endif()
  endif()

  # Install the imgui target (so consumers get it via FeBundleTargets)
  install(TARGETS imgui EXPORT FeBundleTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

  # Install ImGui headers into a stable include path used by INSTALL_INTERFACE
  install(DIRECTORY ${IMGUI_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/FeBundle/3rdparty/imgui
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
    PATTERN "docs" EXCLUDE
    PATTERN "examples" EXCLUDE
    PATTERN ".github" EXCLUDE
  )
endif()

add_library(FeBundle::fe_bundle ALIAS fe_bundle)

# ---- Install & package config ----
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS fe_bundle
  EXPORT FeBundleTargets
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/FeBundleConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/FeBundleConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/FeBundleConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FeBundle
)

install(EXPORT FeBundleTargets
  NAMESPACE FeBundle::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FeBundle
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/FeBundleConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/FeBundleConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FeBundle
)
