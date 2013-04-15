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
#include "clnoise/filter.h"
#include "clnoise/gradientattribute.h"
#include "timer.h"


using namespace CLNoise;
int main(int argc, char *argv[])
{
	try
	{
		Noise noisecl;
		noisecl.initCLContext();
		Generator *source = static_cast<Generator *>(noisecl.createModule("Perlin", BaseModule::GENERATOR));
		Filter *filter = static_cast<Filter*>(noisecl.createModule("ABS", BaseModule::FILTER));
		Output *output = static_cast<Output *>(noisecl.createModule("PlaneMap", BaseModule::OUTPUT));
		Filter *csc = static_cast<Filter*>(noisecl.createModule("gradient", BaseModule::FILTER));
//		Filter *csc = static_cast<Filter*>(noisecl.createModule("grayscale", BaseModule::FILTER));
		if (!source) CL_THROW("Unable to create \"Perlin\" module");
		if (!output) CL_THROW("Unable to create \"PlaneMap\" module");

		Attribute att = csc->getAttribute(0);
		GradientAttribute gradAtt(att.getName());
		
		gradAtt.addPoint(-1.00f, GradientAttribute::GradientPoint(0.12, 0.25, 0.50, 1.00));
		gradAtt.addPoint(-0.20f, GradientAttribute::GradientPoint(0.25, 0.37, 0.50, 1.00));
		gradAtt.addPoint(-0.04f, GradientAttribute::GradientPoint(0.75, 0.75, 0.75, 1.00));
		gradAtt.addPoint( 0.00f, GradientAttribute::GradientPoint(0.00, 0.75, 0.50, 1.00));
		gradAtt.addPoint( 0.25f, GradientAttribute::GradientPoint(0.75, 0.75, 0.00, 1.00));
		gradAtt.addPoint( 0.50f, GradientAttribute::GradientPoint(0.62, 0.37, 0.25, 1.00));
		gradAtt.addPoint( 0.75f, GradientAttribute::GradientPoint(0.50, 1.00, 1.00, 1.00));
		gradAtt.addPoint( 1.00f, GradientAttribute::GradientPoint(1.00, 1.00, 1.00, 1.00));
/*		gradAtt.addPoint(-1.00f, GradientAttribute::GradientPoint(1.00, 0.00, 0.00, 1.00));
		gradAtt.addPoint( 1.00f, GradientAttribute::GradientPoint(1.00, 0.00, 0.00, 1.00));*/
		
		csc->setAttribute(gradAtt);
		
		filter->setInput(0, source);
		csc->setInput(0, filter);
		output->setInput(0, csc);
		output->setImageDimension(256, 256);

		NoiseMap noiseMap(noisecl);

		Timer runTimer, fullTimer;

		noiseMap.build(output);
		uint64_t elapsed = runTimer.get_elapsed_time_us();
		std::cout << "Noise map building:" << elapsed << " us" << std::endl;

		std::ofstream out_cl("output.cl");
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


		std::ofstream out("output.data", std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
		unsigned int w, h;
		output->getImageDimension(&w, &h);
		out.write((char *)output->getData(), w * h * 4);

	}
	catch (const Error &error)
	{
		std::cerr << "====ERROR====" << std::endl;
		std::cerr << error.get_file() << ":" << error.get_line() << " in function \"" << error.get_function() << "\"" << std::endl;
		std::cerr << error.get_reason() << std::endl;
		std::cerr << "=============" << std::endl;

	}
}
