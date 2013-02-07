/*
 *    libnoisecl - procedural noise generation tool based on OpenCL library
 *    Copyright (C) 2013  Denis Biryukov <denis.birukov@gmail.com>
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <iostream>
#include <fstream>

#include "clnoise.h"
#include <clnoiseerror.h>
#include <clnoisemodule.h>
#include <clnoiseoutput.h>
#include "clnoisemap.h"
#include "timer.h"

using namespace CLNoise;
int main(int argc, char *argv[])
{
    try
    {
        Noise noisecl;
        noisecl.initCLContext();
        Module *source = dynamic_cast<Module*>(noisecl.createModule("Perlin", BaseModule::BASE));
        Output *output = dynamic_cast<Output*>(noisecl.createModule("PlaneMap", BaseModule::OUTPUT));
        if ( !source ) THROW("Unable to create \"Perlin\" module");
        if ( !output) THROW("Unable to create \"PlaneMap\" module");

        source->setAttribute("frequency", 1.0f);
        source->setAttribute("lacunarity", 2.0f);
        source->setAttribute("octaveCount", 6);
        source->setAttribute("persistence", 0.5f);
        source->setAttribute("seed", 0);

        output->setInput(0, source);
        output->setImageDimension(256, 256);

	NoiseMap noiseMap(noisecl);

	Timer runTimer, fullTimer;

	noiseMap.build(output);
	uint64_t elapsed = runTimer.get_elapsed_time_us();
	std::cout<<"Noise map building:"<<elapsed<<" us"<<std::endl;

	runTimer.reset();
	noiseMap.allocateResources();
	elapsed = runTimer.get_elapsed_time_us();
	std::cout<<"Noise map resource allocation:"<<elapsed<<" us"<<std::endl;

	runTimer.reset();
	noiseMap.buildKernel();
	elapsed = runTimer.get_elapsed_time_us();
	std::cout<<"Noise map kernel build:"<<elapsed<<" us"<<std::endl;

	runTimer.reset();
	noiseMap.runKernel();
	elapsed = runTimer.get_elapsed_time_us();
	std::cout<<"Noise map kernel run:"<<elapsed<<" us"<<std::endl;

	runTimer.reset();
	noiseMap.transferData();
	elapsed = runTimer.get_elapsed_time_us();
	std::cout<<"Noise map data transfer:"<<elapsed<<" us"<<std::endl;


	std::ofstream out_cl("output.cl");
	out_cl<<noiseMap.getKernelCode();

        std::ofstream out("output.data", std::ios_base::binary|std::ios_base::out|std::ios_base::trunc);
	unsigned int w, h;
	output->getImageDimension(&w, &h);
        out.write((char*)output->getData(), w*h*4);

    }
    catch(const Error &error)
    {
        std::cerr << "====ERROR====" << std::endl;
        std::cerr << error.get_file() << ":" << error.get_line() << " in function \"" << error.get_function() << "\"" << std::endl;
        std::cerr << error.get_reason() << std::endl;
        std::cerr << "=============" << std::endl;

    }
}
