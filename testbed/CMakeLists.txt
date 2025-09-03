# Testbed project that depends on FeBundle
cmake_minimum_required(VERSION 3.20)
project(fe_bundle_testbed LANGUAGES CXX)

# Executável simples
add_executable(fe_bundle_testbed
    src/main.cpp
)

# Força C++23 no testbed também
target_compile_features(fe_bundle_testbed PRIVATE cxx_std_23)

# Linkar contra FeBundle (pelo alias)
target_link_libraries(fe_bundle_testbed PRIVATE FeBundle::fe_bundle)

# Incluir diretórios se precisar de headers públicos
target_include_directories(fe_bundle_testbed PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Se quiser rodar o binário depois de instalar
install(TARGETS fe_bundle_testbed
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
