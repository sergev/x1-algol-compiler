#
# Common includes and libraries for all tests.
#
include_directories(BEFORE ../simulator)
link_libraries(simulator gtest_main)
add_definitions(
    -DBUILD_DIR="${CMAKE_BINARY_DIR}"
    -DTEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

#
# Unit tests.
#
add_executable(x1test EXCLUDE_FROM_ALL
    fixture.cpp
    arch_test.cpp
    arith_test.cpp
    array_test.cpp
    boolean_test.cpp
    cli_test.cpp
    function_test.cpp
    loop_test.cpp
    machine_test.cpp
    procedure_test.cpp
    label_test.cpp
    man_or_boy.cpp
)
add_dependencies(x1test x1sim)
gtest_discover_tests(x1test EXTRA_ARGS --gtest_repeat=1 PROPERTIES TIMEOUT 120)
