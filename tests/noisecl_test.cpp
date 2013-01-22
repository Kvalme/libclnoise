#include <iostream>

#include "noisecl.h"
#include <noiseclerror.h>
#include <noisemodule.h>
#include <noiseoutput.h>

using namespace NOISECL;
int main(int argc, char *argv[])
{
    try
    {
        NoiseCL noisecl;
        NoiseModule *source = noisecl.createModule("Perlin");
        NoiseOutput *output = noisecl.createOutput("PlaneMap");
        if ( !source ) THROW("Unable to create \"Perlin\" module");
        if ( !output) THROW("Unable to create \"PlaneMap\" module");
        
        source->setAttribute("frequency", 1.0f);
        source->setAttribute("lacunarity", 2.0f);
        source->setAttribute("octaveCount", 6);
        source->setAttribute("persistence", 0.5f);
        source->setAttribute("seed", 0);
        
        output->setSource(0, source);
        output->setImageDimension(1024, 1024);
        
    }
    catch(const Error &error)
    {
        std::cerr << "====ERROR====" << std::endl;
        std::cerr << error.get_file() << ":" << error.get_line() << " in function \"" << error.get_function() << "\"" << std::endl;
        std::cerr << error.get_reason() << std::endl;
        std::cerr << "=============" << std::endl;
        
    }
}
