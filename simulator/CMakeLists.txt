# Build library
add_library(simulator STATIC
    x1_arch.cpp
    machine.cpp
    processor.cpp
    opcodes.cpp
    trace.cpp
    virtual_stack.cpp
    encoding.cpp
    input.cpp
)

# Build executable
add_executable(x1sim main.cpp)
target_link_libraries(x1sim simulator)

set_source_files_properties(main.cpp PROPERTIES COMPILE_FLAGS
    -DVERSION_STRING=\\"${CMAKE_PROJECT_VERSION}.${GIT_REVCOUNT}-${GIT_HASH}\\"
)

install(TARGETS x1sim DESTINATION bin)
