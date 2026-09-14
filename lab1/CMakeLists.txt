cmake_minimum_required(VERSION 3.10)
project(Lab1)

# Compile set.h 
add_library(Set STATIC set/set.cpp set/set.h)

# Add the test runner executable
add_executable(run_set_tests set/tests/tests.cpp)
# Add executable for later tests
# add_executable(name files)
# Main CLI executable
add_executable(cli cli.cpp)

# Find and Link UnitTest++ 
find_package(UnitTest++ REQUIRED)

target_link_libraries(run_set_tests PRIVATE Set UnitTest++)
