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

#include <stdio.h>

#include "clnoise/noisemap.h"
#include "clnoise/error.h"
#include "clnoise/output.h"
#include "clnoise/attribute.h"
#include "clnoise/noise.h"
#include "clnoise/filter.h"

#include "library.h"

using namespace CLNoise;

NoiseMap::NoiseMap(const Noise &noise) :
	kernelSource(""),
	buildedOutput(nullptr),
	clNoise(noise),
	outputBuffer(nullptr),
	intAttributesBuffer(nullptr),
	floatAttributesBuffer(nullptr),
	clProgram(nullptr),
	clKernel(nullptr)
{

}

NoiseMap::~NoiseMap()
{
	if (outputBuffer) clReleaseMemObject(outputBuffer);
	if (intAttributesBuffer) clReleaseMemObject(intAttributesBuffer);
	if (floatAttributesBuffer) clReleaseMemObject(floatAttributesBuffer);
	if (clProgram) clReleaseProgram(clProgram);
	if (clKernel) clReleaseKernel(clKernel);
}

void NoiseMap::build(Output *output)
{
	if (!output) CL_THROW("Invalid output passed");

	auto outputInputs = output->getInputs();

	if (output->getInputCount() != outputInputs.size()) CL_THROW("Invalid amount of input modules in output module");

	processedDeps.clear();
	attributeMap.clear();
	intAttributes.clear();
	floatAttributes.clear();
	kernelSources.clear();
	kernelProtos.clear();
	kernelCode.clear();

	output->build(this);

	buildedOutput = output;
	
	buildCode();
}

void NoiseMap::updateAttributes()
{
	for (auto moduleIt : attributeMap)
	{
		for (Attribute attribute : moduleIt.first->getAttributes())
		{
			auto attIt = moduleIt.second.find(attribute.getName());
			if (attIt == moduleIt.second.end()) CL_THROW(std::string("Attribute \"") + attribute.getName() + "\" not listed in attribute map.");

			switch (attribute.getType())
			{
				case Attribute::FLOAT:
					if (floatAttributes.size() <= attIt->second) CL_THROW("Attribute position is out of bounds");
					floatAttributes[attIt->second] = attribute.getFloat();
					break;
				case Attribute::INT:
					if (intAttributes.size() <= attIt->second) CL_THROW("Attribute position is out of bounds");
					intAttributes[attIt->second] = attribute.getInt();
					break;
				default:
					CL_THROW("Invalid module attribute type");
			}
		}
	}
}

void NoiseMap::addAttribute(BaseModule *module, const Attribute &att)
{
	if (module->getType() == BaseModule::FUNCTION) CL_THROW("Modules of type \"Function\" can't have attributes");

	auto attributeMapEntryIt = attributeMap.find(module);
	if (attributeMapEntryIt == attributeMap.end())
	{
		attributeMapEntryIt = attributeMap.insert(std::make_pair(module, std::map<std::string, unsigned>())).first;
	}

	unsigned position;

	//try to find attribute in existed set
	auto attIt = attributeMapEntryIt->second.find(att.getName());
	if (attIt != attributeMapEntryIt->second.end())
	{
		position = attIt->second;
		switch (att.getType())
		{
			case Attribute::FLOAT:
				if (floatAttributes.size() <= position) CL_THROW("Attribute position is out of bounds");
				floatAttributes[position] = att.getFloat();
				break;
			case Attribute::INT:
				if (intAttributes.size() <= position) CL_THROW("Attribute position is out of bounds");
				intAttributes[position] = att.getInt();
				break;
			default:
				CL_THROW("Unknown attribute type");
		}
	}
	else
	{
		switch (att.getType())
		{
			case Attribute::FLOAT:
				position = floatAttributes.size();
				floatAttributes.push_back(att.getFloat());
				break;
			case Attribute::INT:
				position = intAttributes.size();
				intAttributes.push_back(att.getInt());
				break;
			default:
				CL_THROW("Unknown attribute type");
		}
		attributeMapEntryIt->second.insert(std::make_pair(att.getName(), position));
	}
}

void NoiseMap::addDependency(const std::string &depName)
{
	if (processedDeps.find(depName) != processedDeps.end())return;

	BaseModule *mod = Library::getInstance().getModule(depName);
	if (!mod) CL_THROW("Invalid dependency: " + depName);
	if (mod->getType() != BaseModule::FUNCTION) CL_THROW("Only function module can be set as dependency");

	processedDeps.insert(depName);

	Function *function = static_cast<Function *>(mod);
	function->build(this);
}

void NoiseMap::addFunctionPrototype(const std::string &proto)
{
	kernelProtos.append(proto);
	kernelProtos.append(";\n");
}

void NoiseMap::addFunctionSource(const std::string &kSource)
{
	kernelSources.append(kSource);
}

void NoiseMap::addKernelCode(const std::string &code)
{
	kernelCode.append(code);
}

void NoiseMap::buildCode()
{
	kernelSource.clear();
	kernelSource.append(kernelProtos);
	kernelSource.append("\n");
	kernelSource.append(kernelSources);
	kernelSource.append("\n");

	kernelSource.append("__kernel void ");
	kernelSource.append(buildedOutput->getName() + "Kernel");
	kernelSource.append("(__write_only image2d_t output, __global __read_only int *intAttr, __global __read_only float *floatAttr)\n");
	kernelSource.append("{\n");
	kernelSource.append("    int2 coord = (int2)(get_global_id(0), get_global_id(1));\n");
	kernelSource.append("    int2 size = (int2)(get_global_size(0), get_global_size(1));\n");
	kernelSource.append("    float2 pos = (float2)(coord.x / (float)size.x, coord.y / (float)size.y) - 0.5;\n\n");

	kernelSource.append(kernelCode);
	kernelSource.append("\n");

	kernelSource.append("}\n");
}

void NoiseMap::allocateResources()
{
	cl_context context = clNoise.getCLContext();
	cl_int err;

	unsigned int width, height;
	buildedOutput->getImageDimension(&width, &height);

	if (outputBuffer)
	{
		size_t w, h;
		err = clGetImageInfo(outputBuffer, CL_IMAGE_WIDTH, sizeof(size_t), &w, NULL);
		if (err != CL_SUCCESS) CL_THROW("Unable to query outputBuffer width");

		err = clGetImageInfo(outputBuffer, CL_IMAGE_HEIGHT, sizeof(size_t), &h, NULL);
		if (err != CL_SUCCESS) CL_THROW("Unable to query outputBuffer height");

		if (w != width || h != height)
		{
			clReleaseMemObject(outputBuffer);
			outputBuffer = nullptr;
		}
	}

	if (!outputBuffer)
	{
		_cl_image_format format;
		format.image_channel_data_type = CL_UNSIGNED_INT8;
		format.image_channel_order = CL_RGBA;

		outputBuffer = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &format, width, height, 0, NULL, &err);
		if (err != CL_SUCCESS) CL_THROW("Unable to create output image");

		unsigned char *outData = new unsigned char[width * height * 4];
		buildedOutput->setData(outData);
	}

	if (intAttributesBuffer)
	{
		size_t size;
		err = clGetMemObjectInfo(intAttributesBuffer, CL_MEM_SIZE, sizeof(size_t), &size, NULL);
		if (err != CL_SUCCESS) CL_THROW("Unable to query intAttributesBuffer size");

		if (size != intAttributes.size() * sizeof(int))
		{
			clReleaseMemObject(intAttributesBuffer);
			intAttributesBuffer = nullptr;
		}
	}

	if (!intAttributesBuffer)
	{
		intAttributesBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, (intAttributes.size() > 0 ? intAttributes.size() : 1) * sizeof(int), NULL, &err);
		if (err != CL_SUCCESS) CL_THROW("Unable to create intAttributesBuffer");
	}

	if (floatAttributesBuffer)
	{
		size_t size;
		err = clGetMemObjectInfo(floatAttributesBuffer, CL_MEM_SIZE, sizeof(size_t), &size, NULL);
		if (err != CL_SUCCESS) CL_THROW("Unable to query floatAttributesBuffer size");

		if (size != floatAttributes.size() * sizeof(float))
		{
			clReleaseMemObject(floatAttributesBuffer);
			floatAttributesBuffer = nullptr;
		}
	}

	if (!floatAttributesBuffer)
	{
		floatAttributesBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, (floatAttributes.size() > 0 ? floatAttributes.size() : 1) * sizeof(float), NULL, &err);
		if (err != CL_SUCCESS) CL_THROW("Unable to create floatAttributesBuffer");
	}
}
#include <iostream>
void NoiseMap::buildKernel()
{
	if (clProgram) clReleaseProgram(clProgram);
	if (clKernel) clReleaseKernel(clKernel);

	cl_int err;
	const char *src = kernelSource.c_str();
	clProgram = clCreateProgramWithSource(clNoise.getCLContext(), 1, &src, NULL, &err);
	if (!clProgram) CL_THROW("Failed to create compute program!");

	err = clBuildProgram(clProgram, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		size_t len = 0;
		char *buffer;
		clGetProgramBuildInfo(clProgram, clNoise.getCLDevice(), CL_PROGRAM_BUILD_LOG, 0, 0, &len);
		buffer = new char[len+1];
		cl_int err1 = clGetProgramBuildInfo(clProgram, clNoise.getCLDevice(), CL_PROGRAM_BUILD_LOG, len+1, buffer, 0);
		if(err1 != CL_SUCCESS)
		{
			char buf[2048];
			snprintf(buf, 2048, "Double error!\nFailed to build program: %s\nFailed to get reason of failure: %s\n", 
				 getCLError(err), getCLError(err1));
			CL_THROW(buffer);
		}
		else
		{
			CL_THROW(std::string("Failed to build program:\n") + getCLError(err) + "\n" + buffer);
		}
	}

	std::string moduleName(buildedOutput->getName());
	moduleName.append("Kernel");

	clKernel = clCreateKernel(clProgram, moduleName.c_str(), &err);
	if (!clKernel || err != CL_SUCCESS)
	{
		if (clKernel) clReleaseKernel(clKernel);
		CL_THROW(std::string("Failed to create compute kernel!") + getCLError(err));
	}
}

void NoiseMap::runKernel()
{
	//Set arguments
	cl_int err = clSetKernelArg(clKernel, 0, sizeof(cl_mem), &outputBuffer);
	if (err != CL_SUCCESS) CL_THROW("Unable to set kernel arg for output");

	err = clSetKernelArg(clKernel, 1, sizeof(cl_mem), &intAttributesBuffer);
	if (err != CL_SUCCESS) CL_THROW(std::string("Unable to set kernel arg for intAttributes ") + getCLError(err));
	if (!intAttributes.empty())
	{
		err = clEnqueueWriteBuffer(clNoise.getCLCommandQueue(), intAttributesBuffer, CL_FALSE, 0, intAttributes.size() * sizeof(int), intAttributes.data(), 0, NULL, NULL);
		if (err != CL_SUCCESS) CL_THROW("Unable to send intAttributes to OpenCL");
	}

	err = clSetKernelArg(clKernel, 2, sizeof(cl_mem), &floatAttributesBuffer);
	if (err != CL_SUCCESS) CL_THROW(std::string("Unable to set kernel arg for floatAttributes ") + getCLError(err));
	if (!floatAttributes.empty())
	{
		err = clEnqueueWriteBuffer(clNoise.getCLCommandQueue(), floatAttributesBuffer, CL_FALSE, 0, floatAttributes.size() * sizeof(float), floatAttributes.data(), 0, NULL, NULL);
		if (err != CL_SUCCESS) CL_THROW("Unable to send floatAttributes to OpenCL");
	}
	//Run kernel
	unsigned int width, height;
	buildedOutput->getImageDimension(&width, &height);

	size_t global[2] = { (size_t) width, (size_t) height};

	err = clEnqueueNDRangeKernel(clNoise.getCLCommandQueue(), clKernel, 2, NULL, global, 0, 0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		CL_THROW(std::string("Unable to enqueue kernel! ") + getCLError(err));
	}

	err = clFlush(clNoise.getCLCommandQueue());
	if (err != CL_SUCCESS) CL_THROW(std::string("Error on clFlush:") + getCLError(err));
	err = clFinish(clNoise.getCLCommandQueue());
	if (err != CL_SUCCESS) CL_THROW(std::string("Error on clFinish:") + getCLError(err));
}

void NoiseMap::transferData()
{
	unsigned int width, height;
	buildedOutput->getImageDimension(&width, &height);

	size_t origin[3] = {0, 0, 0};
	size_t region[3] = { (size_t) width, (size_t) height, 1};

	cl_int err = clEnqueueReadImage(clNoise.getCLCommandQueue(), outputBuffer, CL_TRUE, origin, region, 0, 0, buildedOutput->getData(), 0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		CL_THROW(std::string("Unable to read buffer!") + getCLError(err));
	}
}

std::map< std::string, unsigned int > NoiseMap::getAttributeMap(BaseModule *module) const
{
	auto it = attributeMap.find(module);
	if (it == attributeMap.end()) CL_THROW("Requested attribute map for invalid module");
	return it->second;
}

