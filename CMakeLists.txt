project(libnoisecl)

add_executable(libnoisecl main.cpp)

install(TARGETS libnoisecl RUNTIME DESTINATION bin)
