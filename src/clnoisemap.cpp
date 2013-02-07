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

#include "clnoisemap.h"
#include "clnoiseerror.h"
#include "clnoiseoutput.h"
#include "clnoiselibrary.h"
#include "clnoisemoduleattribute.h"
#include "clnoise.h"

using namespace CLNoise;

NoiseMap::NoiseMap(const Noise &noise) :
	kernelSource (""),
	buildedOutput (nullptr),
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
    if (outputBuffer) clReleaseMemObject (outputBuffer);
    if (intAttributesBuffer) clReleaseMemObject (intAttributesBuffer);
    if (floatAttributesBuffer) clReleaseMemObject (floatAttributesBuffer);
    if (clProgram) clReleaseProgram (clProgram);
    if (clKernel) clReleaseKernel (clKernel);
}

void NoiseMap::build (Output *output)
{
	if (!output) THROW ("Invalid output passed");

	auto outputInputs = output->getInputs();
	auto outputControls = output->getControls();

	if (output->getInputCount() != outputInputs.size()) THROW ("Invalid amount of input modules in output module");
	if (output->getControlCount() != outputControls.size()) THROW ("Invalid amount of input modules in output module");

	processedDeps.clear();
	attributeMap.clear();
	intAttributes.clear();
	floatAttributes.clear();

	buildedOutput = output;

	std::string modulesProtos, modulesCode, kernelCode;

	for (auto input : outputInputs)
	{
		processModule (input, &modulesProtos, &modulesCode, &kernelCode);
	}

	for (auto control : outputControls)
	{
		processModule (control, &modulesProtos, &modulesCode, &kernelCode);
	}

	modulesProtos.append(output->getProto());
	modulesCode.append(output->getKernelSource());
	generateKernelCode(output, &kernelCode);

	buildCode(modulesProtos, modulesCode, kernelCode);
}

void NoiseMap::updateAttributes()
{
	for (auto moduleIt : attributeMap)
	{
		for (ModuleAttribute attribute : moduleIt.first->getAttributes())
		{
			auto attIt = moduleIt.second.find(attribute.getName());
			if (attIt == moduleIt.second.end()) THROW (std::string("Attribute \"") + attribute.getName() + "\" not listed in attribute map.");

			switch (attribute.getType())
			{
				case ModuleAttribute::FLOAT:
					if (floatAttributes.size() <= attIt->second) THROW ("Attribute position is out of bounds");
					floatAttributes[attIt->second] = attribute.getFloat();
					break;
				case ModuleAttribute::INT:
					if (intAttributes.size() <= attIt->second) THROW ("Attribute position is out of bounds");
					intAttributes[attIt->second] = attribute.getInt();
					break;
				default:
					THROW ("Invalid module attribute type");
			}
		}
	}
}

void NoiseMap::processModule (BaseModule *module, std::string *proto, std::string *code, std::string *kernelCode)
{
	if (!module) THROW ("Unable to process NULL module");
	if (module->getType() == BaseModule::OUTPUT) THROW ("Impossible to have more then one Output module in noise map");

	processDeps (module, proto, code);

	Function *function = dynamic_cast<Function *> (module);
	proto->append (function->getProto());
	code->append (function->getKernelSource());

	if (module->getType() == BaseModule::BASE)
	{
		Module *baseModule = dynamic_cast<Module *> (module);

		for (auto input : baseModule->getInputs())
		{
			processModule (input, proto, code, kernelCode);
		}

		for (auto control : baseModule->getControls())
		{
			processModule (control, proto, code, kernelCode);
		}

		generateAttributes (baseModule);
		generateKernelCode (baseModule, kernelCode);
	}
}

void NoiseMap::processDeps (BaseModule *module, std::string *proto, std::string *code)
{
	for (auto & str : module->getDependencyList())
	{
		if (processedDeps.find (str) != processedDeps.end()) continue;

		BaseModule *mod = Library::getInstance().getModule(str);
		if (!mod) THROW ("Invalid dependency: " + str);
		if (mod->getType() == BaseModule::OUTPUT) THROW ("Output module can't be set as dependency");

		Function *function = dynamic_cast<Function *> (mod);
		proto->append (function->getProto());
		code->append (function->getKernelSource());

		processedDeps.insert (str);

		processDeps(mod, proto, code);
	}
}

void NoiseMap::generateAttributes (Module *module)
{
	if (module->getType() == BaseModule::FUNCTION) THROW ("Modules of type \"Function\" can't have attributes");

	auto attributeMapEntryIt = attributeMap.find (module);
	if (attributeMapEntryIt == attributeMap.end())
	{
		attributeMapEntryIt = attributeMap.insert (std::make_pair (module, std::map<std::string, unsigned>())).first;
	}

	for (ModuleAttribute attribute : module->getAttributes())
	{
		unsigned position;

		//try to find attribute in existed set
		auto attIt = attributeMapEntryIt->second.find (attribute.getName());
		if (attIt != attributeMapEntryIt->second.end())
		{
			position = attIt->second;
			switch (attribute.getType())
			{
				case ModuleAttribute::FLOAT:
					if (floatAttributes.size() <= position) THROW ("Attribute position is out of bounds");
					floatAttributes[position] = attribute.getFloat();
					break;
				case ModuleAttribute::INT:
					if (intAttributes.size() <= position) THROW ("Attribute position is out of bounds");
					intAttributes[position] = attribute.getInt();
					break;
				default:
					THROW ("Unknown attribute type");
			}
		}
		else
		{
			switch (attribute.getType())
			{
				case ModuleAttribute::FLOAT:
					position = floatAttributes.size();
					floatAttributes.push_back (attribute.getFloat());
					break;
				case ModuleAttribute::INT:
					position = intAttributes.size();
					intAttributes.push_back (attribute.getInt());
					break;
				default:
					THROW ("Unknown attribute type");
			}
			attributeMapEntryIt->second.insert(std::make_pair(attribute.getName(), position));
		}
	}
}

void NoiseMap::generateKernelCode (Module *module, std::string *kernelCode)
{
	if (module->getType() != BaseModule::OUTPUT)
	{
		kernelCode->append(std::string("    float ") + module->getName() + "Out = " + module->getName() + "( ");
	}
	else
	{
		kernelCode->append(std::string("    ") + module->getName() + "( ");
	}
	std::vector<std::string> args;

	if(module->getType() == BaseModule::OUTPUT)
	{
		args.push_back("coord");
	}
	//Generator module - pass position as first argument
	if(module->getInputCount() == 0)
	{
		args.push_back("position");
	}
	else
	{
		for ( auto input : module->getInputs())
		{
			args.push_back(input->getName() + "Out");
		}
	}
	for ( auto control : module->getControls())
	{
		args.push_back(control->getName() + "Out");
	}

	auto attributeMapEntryIt = attributeMap.find(module);
	if ( module->getAttributeCount() != 0 && attributeMapEntryIt == attributeMap.end()) THROW ("Unable to found attribute map for module");

	for ( ModuleAttribute att : module->getAttributes())
	{
		auto attInfoIt = attributeMapEntryIt->second.find(att.getName());
		if(attInfoIt == attributeMapEntryIt->second.end()) THROW ("Unmapped attribute");

		char value[256];

		switch (att.getType())
		{
			case ModuleAttribute::FLOAT:
				snprintf(value, 255, "floatAttributes[%d]", attInfoIt->second);
				break;
			case ModuleAttribute::INT:
				snprintf(value, 255, "intAttributes[%d]", attInfoIt->second);
				break;
			default:
				THROW ("Unknown attribute type");
		}
		args.push_back(value);
	}

	if (module->getType() == BaseModule::OUTPUT)
	{
		args.push_back("output");
	}

	bool isFirst = true;
	for (auto str : args)
	{
		if (!isFirst) kernelCode->append(", ");
		kernelCode->append(str);
		isFirst = false;
	}
	kernelCode->append(");\n");
}

void NoiseMap::buildCode (const std::string &proto, const std::string &code, const std::string &kernelCode)
{
	kernelSource.clear();
	kernelSource.append(proto);
	kernelSource.append("\n");
	kernelSource.append(code);
	kernelSource.append("\n");

	kernelSource.append("__kernel void ");
	kernelSource.append(buildedOutput->getName()+"Kernel");
	kernelSource.append("(__write_only __global image2d_t output, __read_only __global int *intAttributes, __read_only __global float *floatAttributes)\n");
	kernelSource.append("{\n");
	kernelSource.append("    int2 coord = (int2)(get_global_id(0), get_global_id(1));\n");
	kernelSource.append("    int2 size = (int2)(get_global_size(0), get_global_size(1));\n");
	kernelSource.append("    float2 position = (float2)(coord.x / (float)size.x, coord.y / (float)size.y);\n");

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
		err = clGetImageInfo(outputBuffer, CL_IMAGE_WIDTH, sizeof(size_t), &w, NULL );
		if (err != CL_SUCCESS) THROW ("Unable to query outputBuffer width");

		err = clGetImageInfo(outputBuffer, CL_IMAGE_HEIGHT, sizeof(size_t), &h, NULL );
		if (err != CL_SUCCESS) THROW ("Unable to query outputBuffer height");

		if (w!=width || h != height)
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

		outputBuffer = clCreateImage2D ( context, CL_MEM_WRITE_ONLY, &format, width, height, 0, NULL, &err );
		if ( err != CL_SUCCESS ) THROW ( "Unable to create output image" );

		unsigned char *outData = new unsigned char[width * height * 4];
		buildedOutput->setData(outData);
	}

	if (intAttributesBuffer)
	{
		size_t size;
		err = clGetMemObjectInfo(intAttributesBuffer, CL_MEM_SIZE, sizeof(size_t), &size, NULL);
		if (err != CL_SUCCESS) THROW ("Unable to query intAttributesBuffer size");

		if (size != intAttributes.size() * sizeof(int))
		{
			clReleaseMemObject(intAttributesBuffer);
			intAttributesBuffer = nullptr;
		}
	}

	if (!intAttributesBuffer)
	{
		intAttributesBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, intAttributes.size() * sizeof(int), NULL, &err);
		if ( err != CL_SUCCESS ) THROW ( "Unable to create intAttributesBuffer" );
	}

	if (floatAttributesBuffer)
	{
		size_t size;
		err = clGetMemObjectInfo(floatAttributesBuffer, CL_MEM_SIZE, sizeof(size_t), &size, NULL);
		if (err != CL_SUCCESS) THROW ("Unable to query floatAttributesBuffer size");

		if (size != floatAttributes.size() * sizeof(float))
		{
			clReleaseMemObject(floatAttributesBuffer);
			floatAttributesBuffer = nullptr;
		}
	}

	if (!floatAttributesBuffer)
	{
		floatAttributesBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, floatAttributes.size() * sizeof(float), NULL, &err);
		if ( err != CL_SUCCESS ) THROW ( "Unable to create floatAttributesBuffer" );
	}
}

void NoiseMap::buildKernel()
{
	if ( clProgram ) clReleaseProgram ( clProgram );
	if ( clKernel ) clReleaseKernel ( clKernel );

	cl_int err;
	const char *src = kernelSource.c_str();
	clProgram = clCreateProgramWithSource ( clNoise.getCLContext(), 1, &src, NULL, &err );
	if ( !clProgram ) THROW ( "Failed to create compute program!" );

	err = clBuildProgram ( clProgram, 0, NULL, NULL, NULL, NULL );
	if ( err != CL_SUCCESS )
	{
		size_t len;
		char buffer[2048];
		clGetProgramBuildInfo ( clProgram, clNoise.getCLDevice(), CL_PROGRAM_BUILD_LOG, sizeof ( buffer ), buffer, &len );
		THROW ( std::string ( "Failed to build program:\n" ) + buffer );
	}

	std::string moduleName(buildedOutput->getName());
	moduleName.append("Kernel");

	clKernel = clCreateKernel ( clProgram, moduleName.c_str(), &err );
	if ( !clKernel || err != CL_SUCCESS )
	{
		if ( clKernel ) clReleaseKernel ( clKernel );
		THROW ( std::string ( "Failed to create compute kernel!" ) + getCLError ( err ) );
	}
}

void NoiseMap::runKernel()
{
	//Set arguments
	cl_int err = clSetKernelArg ( clKernel, 0, sizeof ( cl_mem ), &outputBuffer );
	if ( err != CL_SUCCESS ) THROW ( "Unable to set kernel arg for output" );

	err = clSetKernelArg ( clKernel, 1, sizeof ( cl_mem ), &intAttributesBuffer);
	if ( err != CL_SUCCESS ) THROW ( std::string("Unable to set kernel arg for intAttributes ") + getCLError(err) );
	err = clEnqueueWriteBuffer ( clNoise.getCLCommandQueue(), intAttributesBuffer, CL_FALSE, 0, intAttributes.size() * sizeof(int), intAttributes.data(), 0, NULL, NULL);
	if ( err != CL_SUCCESS ) THROW ( "Unable to send intAttributes to OpenCL" );

	err = clSetKernelArg ( clKernel, 2, sizeof ( cl_mem ), &floatAttributesBuffer);
	if ( err != CL_SUCCESS ) THROW ( std::string("Unable to set kernel arg for floatAttributes ") + getCLError(err) );
	err = clEnqueueWriteBuffer ( clNoise.getCLCommandQueue(), floatAttributesBuffer, CL_FALSE, 0, floatAttributes.size() * sizeof(float), floatAttributes.data(), 0, NULL, NULL);
	if ( err != CL_SUCCESS ) THROW ( "Unable to send floatAttributes to OpenCL" );

	//Run kernel
	unsigned int width, height;
	buildedOutput->getImageDimension(&width, &height);

	size_t global[2] = { ( size_t ) width, ( size_t ) height};

	err = clEnqueueNDRangeKernel ( clNoise.getCLCommandQueue(), clKernel, 2, NULL, global, 0, 0, NULL, NULL );
	if ( err != CL_SUCCESS )
	{
		THROW ( std::string ( "Unable to enqueue kernel! " ) + getCLError ( err ) );
	}

	err = clFlush ( clNoise.getCLCommandQueue() );
	if ( err != CL_SUCCESS ) THROW (std::string("Error on clFlush:") + getCLError(err));
	err = clFinish ( clNoise.getCLCommandQueue() );
	if ( err != CL_SUCCESS ) THROW (std::string("Error on clFinish:") + getCLError(err));
}

void NoiseMap::transferData()
{
	unsigned int width, height;
	buildedOutput->getImageDimension(&width, &height);

	size_t origin[3] = {0, 0, 0};
	size_t region[3] = { ( size_t ) width, ( size_t ) height, 1};

	cl_int err = clEnqueueReadImage ( clNoise.getCLCommandQueue(), outputBuffer, CL_TRUE, origin, region, 0, 0, buildedOutput->getData(), 0, NULL, NULL );
	if ( err != CL_SUCCESS )
	{
		THROW ( std::string ( "Unable to read buffer!" ) + getCLError ( err ) );
	}
}
