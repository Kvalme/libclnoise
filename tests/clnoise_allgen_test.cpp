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

#include "clnoise/noise.h"
#include "clnoise/error.h"
#include "clnoise/generator.h"
#include "clnoise/output.h"
#include "clnoise/noisemap.h"
#include "timer.h"

using namespace CLNoise;
int main(int argc, char *argv[])
{
	try
	{
		Noise noisecl;
		noisecl.initCLContext();
		Generator *source;
		Output *output = static_cast<Output *>(noisecl.createModule("PlaneMap", BaseModule::OUTPUT));
		if (!output) CL_THROW("Unable to create \"PlaneMap\" module");

		for( auto genName : noisecl.getModulesOfType(BaseModule::GENERATOR))
		{
			
			std::cout<<"========= Testing "<<genName<<" ========="<<std::endl;
			
			source = static_cast<Generator *>(noisecl.createModule(genName, BaseModule::GENERATOR));
			if (!source) CL_THROW("Unable to create \"Perlin\" module");
			
			output->setInput(0, source);
			output->setImageDimension(256, 256);

			NoiseMap noiseMap(noisecl);

			Timer runTimer, fullTimer;

			noiseMap.build(output);
			uint64_t elapsed = runTimer.get_elapsed_time_us();
			std::cout << "Noise map building:" << elapsed << " us" << std::endl;

			std::string clName = genName;
			clName.append(".cl");
			std::ofstream out_cl(clName);
			out_cl << noiseMap.getKernelCode();

			runTimer.reset();
			noiseMap.allocateResources();
			elapsed = runTimer.get_elapsed_time_us();
			std::cout << "Noise map resource allocation:" << elapsed << " us" << std::endl;

			runTimer.reset();
			noiseMap.buildKernel();
			elapsed = runTimer.get_elapsed_time_us();
			std::cout << "Noise map kernel build:" << elapsed << " us" << std::endl;
			
			std::ofstream clOut("output.cl", std::ios_base::out | std::ios_base::trunc);
			clOut<<noiseMap.getKernelCode();
			
			runTimer.reset();
			noiseMap.runKernel();
			elapsed = runTimer.get_elapsed_time_us();
			std::cout << "Noise map kernel run:" << elapsed << " us" << std::endl;

			runTimer.reset();
			noiseMap.transferData();
			elapsed = runTimer.get_elapsed_time_us();
			std::cout << "Noise map data transfer:" << elapsed << " us" << std::endl;

			std::string outName = genName;
			outName.append("256x256.data");
			std::ofstream out(outName, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
			unsigned int w, h;
			output->getImageDimension(&w, &h);
			out.write((char *)output->getData(), w * h * 4);
			
			std::cout<<"========= Done "<<genName<<" ========="<<std::endl;
		}
	}
	catch (const Error &error)
	{
		std::cerr << "====ERROR====" << std::endl;
		std::cerr << error.get_file() << ":" << error.get_line() << " in function \"" << error.get_function() << "\"" << std::endl;
		std::cerr << error.get_reason() << std::endl;
		std::cerr << "=============" << std::endl;

	}
}
