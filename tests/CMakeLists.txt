add_executable(speed_benchmark speed_benchmark.cpp)
target_link_libraries(speed_benchmark OpenCL noise)

include_directories(${libclnoise_SOURCE_DIR}/src)

add_executable(clnoise_test clnoise_test.cpp)
target_link_libraries(clnoise_test clnoise)