set(clnoise_SOURCES clnoiseoutput.cpp clnoisemodule.cpp clnoise.cpp)
add_library(clnoise SHARED ${clnoise_SOURCES})
target_link_libraries(clnoise OpenCL)
