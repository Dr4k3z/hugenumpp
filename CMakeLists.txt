cmake_minimum_required(VERSION 3.31)
project(hugenumpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(hugenum STATIC
    src/LargeNum.cpp src/Algorithms.cpp src/MemoryLogger.cpp
)

if (CMAKE_BUILD_TYPE STREQUAL "MemoryDiagnostic")
    add_definitions(-DENABLE_MEMORY_DIAGNOSTICS)
    message(STATUS "Memory Diagnostics Enabled for MemoryDiagnostic build type.")
endif()

target_include_directories(hugenum PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

add_executable(hugenumpp main.cpp)
target_link_libraries(hugenumpp PUBLIC hugenum)

# --- Google Test ---
enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/../googletest googletest-build)

add_executable(unit_tests tests/test_example.cpp tests/test_algorithms.cpp)

target_link_libraries(unit_tests gtest gtest_main hugenum)

add_test(NAME UnitTests COMMAND unit_tests)