set(noisecl_SOURCES noiseoutput.cpp noisemodule.cpp noisecl.cpp)
add_library(noisecl SHARED ${noisecl_SOURCES})
target_link_libraries(noisecl OpenCL)
