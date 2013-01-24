#include <iostream>
#include <fstream>

#include "clnoise.h"
#include <clnoiseerror.h>
#include <clnoisemodule.h>
#include <clnoiseoutput.h>

using namespace CLNoise;
int main(int argc, char *argv[])
{
    try
    {
        Noise noisecl;
        noisecl.initCLContext();
        Module *source = noisecl.createModule("Perlin");
        Output *output = noisecl.createOutput("PlaneMap");
        if ( !source ) THROW("Unable to create \"Perlin\" module");
        if ( !output) THROW("Unable to create \"PlaneMap\" module");
        
        source->setAttribute("frequency", 1.0f);
        source->setAttribute("lacunarity", 2.0f);
        source->setAttribute("octaveCount", 6);
        source->setAttribute("persistence", 0.5f);
        source->setAttribute("seed", 0);

        output->setSource(0, source);
        output->setImageDimension(1024, 1024);
        output->build();
        std::cout<<output->getBuildedSource()<<std::endl;
        unsigned char *image = new unsigned char[1024*1024*4];
        output->run();
        output->getImage(image);

        std::ofstream out("output.raw", std::ios_base::binary|std::ios_base::out|std::ios_base::trunc);
        out.write((char*)image, 1024*1024*4);
        
    }
    catch(const Error &error)
    {
        std::cerr << "====ERROR====" << std::endl;
        std::cerr << error.get_file() << ":" << error.get_line() << " in function \"" << error.get_function() << "\"" << std::endl;
        std::cerr << error.get_reason() << std::endl;
        std::cerr << "=============" << std::endl;
        
    }
}
