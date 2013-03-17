add_executable(speed_benchmark speed_benchmark.cpp)
target_link_libraries(speed_benchmark OpenCL noise rt)

include_directories(${libclnoise_SOURCE_DIR}/headers)

add_executable(clnoise_test clnoise_test.cpp)
target_link_libraries(clnoise_test clnoise rt)

add_executable(clnoise_allgen_test clnoise_allgen_test.cpp)
target_link_libraries(clnoise_allgen_test clnoise rt)
