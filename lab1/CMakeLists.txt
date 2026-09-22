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

# Treat fetched UnitTest++ as requiring at cleast CMake 3.5
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(UTPP_BUILD_TESTS OFF CACHE BOOL "Disable internal UnitTest++ tests" FORCE)
set(UTPP_BUILD_SAMPLES OFF CACHE BOOL "Disable internal UnitTest++ samples" FORCE)

# Fetch and Link UnitTest++
include(FetchContent)
FetchContent_Declare(
    UnitTestPlusPlus
    GIT_REPOSITORY https://github.com/unittest-cpp/unittest-cpp.git
    GIT_TAG        v2.0.0
)

FetchContent_MakeAvailable(UnitTestPlusPlus)

target_link_libraries(run_set_tests PRIVATE Set UnitTest++)
target_link_libraries(run_rubik_tests PRIVATE Rubik UnitTest++)
target_link_libraries(cli PRIVATE Set Rubik)

enable_testing()
add_test(NAME SetTests COMMAND run_set_tests)
add_test(NAME RubikTests COMMAND run_rubik_tests)