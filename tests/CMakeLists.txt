cmake_minimum_required(VERSION 2.8)

set(CMAKE_C_FLAGS_DEBUG "-fPIC -DPIC -Wall -Werror -O0 -g3 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-g0 -O3 -DNDEBUG -mfpmath=sse,387 -msse -msse2 -msse3 -msse4 -mtune=corei7-avx -march=corei7-avx")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++0x -fPIC -DPIC -Wall -Werror -O0 -g3 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++0x -g0 -O3 -DNDEBUG -mfpmath=sse,387 -msse -msse2 -msse3 -msse4 -mtune=corei7-avx -march=corei7-avx")


add_executable(speed_benchmark speed_benchmark.cpp)
target_link_libraries(speed_benchmark OpenCL noise)

add_executable(noisecl_test noisecl_test.cpp)
target_link_libraries(speed_benchmark noisecl)