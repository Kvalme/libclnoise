include_directories(${libclnoise_SOURCE_DIR}/headers)
set(clnoise_SOURCES output.cpp generator.cpp noise.cpp basemodule.cpp function.cpp library.cpp noisemap.cpp filter.cpp gradientattribute.cpp attribute.cpp)
add_library(clnoise SHARED ${clnoise_SOURCES})
target_link_libraries(clnoise OpenCL)
