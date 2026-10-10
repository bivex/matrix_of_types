cmake_minimum_required(VERSION 3.20)
project(DslHexDddLibrary VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wno-sign-conversion)
endif()

# Header-only interface library for the DSL
add_library(dsl_lib INTERFACE)
target_include_directories(dsl_lib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Test Executable
add_executable(dsl_tests tests/test_dsl.cpp)
target_link_libraries(dsl_tests PRIVATE dsl_lib)

# Demo Executable
add_executable(dsl_demo examples/main.cpp)
target_link_libraries(dsl_demo PRIVATE dsl_lib)

# Enable CTest
enable_testing()
add_test(NAME dsl_unit_tests COMMAND dsl_tests)
