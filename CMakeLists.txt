cmake_minimum_required(VERSION 2.8)

project(libclnoise)

set(CMAKE_C_FLAGS_DEBUG "-fPIC -DPIC -Wall -Werror -O0 -g3 -DDEBUG -Wno-cpp")
set(CMAKE_C_FLAGS_RELEASE "-fPIC -DPIC -g0 -O3 -Wall -Werror -DNDEBUG -mfpmath=sse,387 -msse -msse2 -msse3 -Wno-cpp")
set(CMAKE_CXX_FLAGS_DEBUG "-std=c++0x -fPIC -DPIC -Wall -Werror -O0 -g3 -DDEBUG -Wno-cpp")
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++0x -fPIC -DPIC -Wall -Werror -g0 -O3 -DNDEBUG -mfpmath=sse,387 -msse -msse2 -msse3 -Wno-cpp")

subdirs(src)
subdirs(tests)

