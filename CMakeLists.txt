cmake_minimum_required(VERSION 3.20)
project(FeBundle VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)

# ---- Global C++23 + tooling ----
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- Options ----
option(FEBUNDLE_COMPILED "Build FeBundle as a compiled library (not header-only)" ON)
option(FEBUNDLE_BUILD_SHARED "If compiled, build as shared (otherwise static)" OFF)
option(FEBUNDLE_CCDB "Generate a TU for compile_commands.json in header-only mode" ON)
option(FEBUNDLE_WITH_IMGUI "Enable Dear ImGui integration" ON)
option(FEBUNDLE_IMGUI_DEMO "Compile imgui_demo.cpp" OFF)
option(FEBUNDLE_IMGUI_SDL3_GL3 "Enable ImGui SDL3 + OpenGL3 backends" OFF)

# ---- Public headers ----
set(FEBUNDLE_HEADERS
    include/FeBundle/FeBundle.hpp
    include/FeBundle/Core/Logger.hpp
    include/FeBundle/Core/Assets/Common.hpp
    include/FeBundle/Core/Assets/Texture.hpp
    include/FeBundle/Core/Assets/Audio.hpp
    include/FeBundle/Core/Defines.hpp
    include/FeBundle/Core/Memory/Memory.hpp
    include/FeBundle/Core/Window/Window.hpp
    include/FeBundle/Core/Renderer/Renderer.hpp
    include/FeBundle/Core/Renderer/ImGuiLayer.hpp
    include/FeBundle/Core/Scene/Entity.hpp
    include/FeBundle/Core/Scene/Components.hpp
    include/FeBundle/Core/Scene/Store.hpp
    include/FeBundle/Core/Scene/Scene.hpp
    include/FeBundle/Core/UI/Widget.hpp
    include/FeBundle/Core/UI/Button.hpp
    include/FeBundle/Core/Audio/AudioMixer.hpp
    include/FeBundle/Core/Camera/Camera.hpp
    include/FeBundle/Core/Systems/InputManager.hpp
    include/FeBundle/Core/Systems/AssetManager.hpp
    include/FeBundle/Core/Systems/AssetLoader.hpp
    include/FeBundle/Core/Systems/RenderSystem.hpp
    include/FeBundle/Core/Systems/FileWatcher.hpp
    include/FeBundle/Core/Systems/CollisionSystem.hpp
    include/FeBundle/Core/Systems/CameraSystem.hpp
    include/FeBundle/Core/Systems/AudioSystem.hpp
    include/FeBundle/Core/Systems/UIManager.hpp
    include/FeBundle/Core/Events/GameCommandEvent.hpp
    include/FeBundle/Core/Events/RenderEvents.hpp
    include/FeBundle/Core/Events/EventQueue.hpp
    include/FeBundle/Core/Events/EventBus.hpp
    include/FeBundle/Core/Commands/AudioCommands.hpp
    include/FeBundle/Core/Events/AssetLoadEvent.hpp
    include/FeBundle/Core/Clock.hpp
    include/FeBundle/Core/GameBridge.hpp
    include/FeBundle/Core/Math/Math.hpp
    include/FeBundle/Core/GameTypes.hpp
    include/FeBundle/Core/Entrypoint.hpp
    include/FeBundle/Core/Application.hpp
)

# ---- Sources (only used when compiled) ----
set(FEBUNDLE_SOURCES
    src/Core/Memory/Memory.cc
    src/Core/Window/Window.cc
    src/Core/Renderer/Renderer.cc
    src/Core/Math/Math.cc
    src/Core/Renderer/ImGuiLayer.cc
    src/Core/Scene/Scene.cc
    src/Core/Systems/InputManager.cc
    src/Core/Systems/AssetLoader.cc
    src/Core/Systems/FileWatcher.cc
    src/Core/Systems/AssetManager.cc
    src/Core/Systems/RenderSystem.cc
    src/Core/Systems/CameraSystem.cc
    src/Core/Audio/AudioMixer.cc
    src/Core/Camera/Camera.cc
    src/Core/Systems/AudioSystem.cc
    src/Core/Systems/UIManager.cc
    src/Core/Systems/CollisionSystem.cc
    src/Core/Clock.cc
    src/Core/Game.cc
	src/Core/GameBridge.cc
    src/Core/Application.cc
)

# ---- FeBundle library ----
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
  set_target_properties(fe_bundle PROPERTIES POSITION_INDEPENDENT_CODE ON)
else()
  add_library(fe_bundle INTERFACE)
  target_compile_features(fe_bundle INTERFACE cxx_std_23)
  target_include_directories(fe_bundle
    INTERFACE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
  )
endif()

# ---- Debug macro ----
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(fe_bundle PUBLIC FE_DEBUG)
endif()

# ---- stduuid (header-only) ----
set(STDUUID_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern/stduuid)
target_include_directories(fe_bundle PUBLIC
  $<BUILD_INTERFACE:${STDUUID_DIR}>
  $<INSTALL_INTERFACE:include/FeBundle/3rdparty/stduuid>
)

install(DIRECTORY ${STDUUID_DIR}/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/FeBundle/3rdparty/stduuid
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# ---- ImGui integration ----
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

  set(IMGUI_BACKENDS
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer3.cpp
  )

  add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_BACKENDS})
  target_compile_features(imgui PUBLIC cxx_std_23)
  target_include_directories(imgui PUBLIC
    $<BUILD_INTERFACE:${IMGUI_DIR}>
    $<BUILD_INTERFACE:${IMGUI_DIR}/backends>
    $<INSTALL_INTERFACE:include/FeBundle/3rdparty/imgui>
    $<INSTALL_INTERFACE:include/FeBundle/3rdparty/imgui/backends>
  )

  find_package(SDL3 CONFIG REQUIRED)
  find_package(SDL3_image CONFIG REQUIRED)
  target_link_libraries(imgui PUBLIC SDL3::SDL3)

  target_link_libraries(fe_bundle PUBLIC imgui)

  install(TARGETS imgui EXPORT FeBundleTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )

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

target_link_libraries(fe_bundle PUBLIC SDL3::SDL3 SDL3_image::SDL3_image)

add_subdirectory(testbed)
add_subdirectory(tests)
