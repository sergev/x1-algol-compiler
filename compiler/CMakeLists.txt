#
# Build Pascal version
#
add_custom_target(x1algol
    COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR} pascal DESTDIR=${CMAKE_CURRENT_BINARY_DIR}
)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/x1algol DESTINATION bin)

#
# Build C version
#
add_executable(x1algc x1algc.c)
install(TARGETS x1algc DESTINATION bin)
