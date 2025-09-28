cmake_minimum_required(VERSION 3.31)
project(hugenumpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(hugenum STATIC
    src/LargeNum.cpp
)

target_include_directories(hugenum PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

add_executable(hugenumpp main.cpp)
# add_executable(prototype prototype.cpp)
target_link_libraries(hugenumpp PUBLIC hugenum)

# --- Google Test ---
enable_testing()
add_subdirectory(${CMAKE_SOURCE_DIR}/../googletest googletest-build)

add_executable(test_example tests/test_example.cpp)

target_link_libraries(test_example gtest gtest_main hugenum)

add_test(NAME ExampleTest COMMAND test_example)