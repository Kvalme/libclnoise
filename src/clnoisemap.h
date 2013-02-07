/*
    libnoisecl - procedural noise generation tool based on OpenCL library
    Copyright (C) 2013  Denis Biryukov <denis.birukov@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#pragma once
#include <string>
#include <map>
#include <set>
#include <vector>

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <CL/opencl.h>

namespace CLNoise
{

class Module;
class BaseModule;
class Output;
class Noise;

class NoiseMap
{
public:
	NoiseMap(const Noise &noise);
	~NoiseMap();
	void build (Output *output);
	void updateAttributes();

	void allocateResources();
	void buildKernel();
	void runKernel();
	void transferData();

	std::string getKernelCode() const { return kernelSource;}

protected:
	void processModule (BaseModule *module, std::string *proto, std::string *code, std::string *kernelCode);
	void processDeps (BaseModule *module, std::string *proto, std::string *code);
	void generateAttributes (Module *module);
	void generateKernelCode (Module *module, std::string *kernelCode);
	void buildCode(const std::string &proto, const std::string &code, const std::string &kernelCode);

	std::string kernelSource;
	Output *buildedOutput;
	std::map<Module *, std::map<std::string, unsigned> > attributeMap;
	std::set<std::string> processedDeps;

	std::vector<int> intAttributes;
	std::vector<float> floatAttributes;

	const Noise &clNoise;

	cl_mem outputBuffer;
	cl_mem intAttributesBuffer;
	cl_mem floatAttributesBuffer;

	cl_program clProgram;
	cl_kernel clKernel;
};

}
