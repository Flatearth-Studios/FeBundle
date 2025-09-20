# Testbed project that depends on FeBundle
cmake_minimum_required(VERSION 3.20)
project(fe_bundle_unit_tests LANGUAGES CXX)

add_executable(fe_bundle_unit_tests
    src/main.cpp
    src/TestManager.cpp
    src/Math/MathTests.cpp
)

target_compile_features(fe_bundle_unit_tests PRIVATE cxx_std_23)

target_link_libraries(fe_bundle_unit_tests PRIVATE FeBundle::fe_bundle)

target_include_directories(fe_bundle_unit_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

install(TARGETS fe_bundle_unit_tests
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
