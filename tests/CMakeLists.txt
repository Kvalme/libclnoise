add_executable(speed_benchmark speed_benchmark.cpp)
target_link_libraries(speed_benchmark OpenCL noise)

include_directories(${libnoisecl_SOURCE_DIR}/src)

add_executable(noisecl_test noisecl_test.cpp)
target_link_libraries(noisecl_test noisecl)