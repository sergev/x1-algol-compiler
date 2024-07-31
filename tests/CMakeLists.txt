#
# Common includes and libraries for all tests.
#
include_directories(BEFORE ../simulator)
link_libraries(simulator gtest_main)
add_definitions(-DTEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

#
# Unit tests.
#
add_executable(x1test EXCLUDE_FROM_ALL
    fixture.cpp
    cli_test.cpp
    machine_test.cpp
)
add_dependencies(x1test x1sim)
gtest_discover_tests(x1test EXTRA_ARGS --gtest_repeat=1 PROPERTIES TIMEOUT 120)
