cmake_minimum_required(VERSION 3.10)
project(Lab1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set library
add_library(Set STATIC set/set.cpp set/set.h)

# Rubik's Cube library
add_library(Rubik STATIC rubik/rubik.cpp rubik/rubik.h)

# Test runner for the Set library
add_executable(run_set_tests set/tests/tests.cpp)

# Test runner for the Rubik library
add_executable(run_rubik_tests rubik/tests/tests.cpp)

# Main CLI executable demonstrating both libraries
add_executable(cli cli.cpp)

# Find and Link UnitTest++
find_package(UnitTest++ REQUIRED)

target_link_libraries(run_set_tests PRIVATE Set UnitTest++)
target_link_libraries(run_rubik_tests PRIVATE Rubik UnitTest++)
target_link_libraries(cli PRIVATE Set Rubik)