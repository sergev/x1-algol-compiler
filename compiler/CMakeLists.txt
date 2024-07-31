#
# Build Pascal version
#
add_executable(x1algol x1algol.pas)
install(TARGETS x1algol DESTINATION bin)

#
# Build C version
#
add_executable(x1algc x1algc.c)
install(TARGETS x1algc DESTINATION bin)
