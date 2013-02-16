/*
 *    libnoisecl - procedural noise generation tool based on OpenCL library
 *    Copyright (C) 2013  Messenger of death <messengerofdeath@gmail.com>
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

#include <algorithm>

#include "clnoise.h"
#include "clnoisemodule.h"
#include "clnoiseoutput.h"
#include "clnoisefunction.h"
#include "clnoiselibrary.h"
#include "clnoisemap.h"


using namespace CLNoise;


Noise::Noise()
{
	isCLAllocatedInternally = false;
    clCommands = 0;
    clDeviceId = 0;
    clContext = 0;
}

Noise::~Noise()
{
	clReleaseCommandQueue(clCommands);
	if (isCLAllocatedInternally)
	{
		clReleaseContext(clContext);
	}
}

std::vector<std::string> Noise::getModulesOfType(BaseModule::MODULE_TYPE type)
{
	return Library::getInstance().getModulesOfType(type);
}

BaseModule *Noise::createModule(const std::string &name, BaseModule::MODULE_TYPE type)
{
	return Library::getInstance().createModule(name, type);
}

void Noise::initCLContext()
{
	// Connect to a compute device
	cl_int err = CL_SUCCESS;
	cl_platform_id clPlatform;
	err = clGetPlatformIDs(1, &clPlatform, 0);
	if (err != CL_SUCCESS) CL_THROW(std::string("Failed to find a platform: ") + getCLError(err));

	// Get a device of the appropriate type
	err = clGetDeviceIDs(clPlatform, CL_DEVICE_TYPE_GPU , 1, &clDeviceId, 0);
	if (err != CL_SUCCESS) CL_THROW(std::string("Failed to create device group: ") + getCLError(err));

	// Create a compute context
	clContext = clCreateContext(0, 1, &clDeviceId, NULL, NULL, &err);
	if (!clContext) CL_THROW(std::string("Failed to create compute context: ") + getCLError(err));

	clCommands = clCreateCommandQueue(clContext, clDeviceId, 0, &err);
	if (!clCommands) CL_THROW(std::string("Failed to create command queue: ") + getCLError(err));
	
	isCLAllocatedInternally = true;
}

void Noise::setCLDevice(cl_device_id device, cl_context context)
{
	if (!context) CL_THROW("Invalid context passed");
	if (!device)  CL_THROW("Invalid device passed");
	
	if (clCommands) clReleaseCommandQueue(clCommands);
	if (clContext) clReleaseContext(clContext);
	
	clContext = context;
	clDeviceId = device;

	cl_int err = CL_SUCCESS;
	clCommands = clCreateCommandQueue(clContext, clDeviceId, 0, &err);
	if (!clCommands) CL_THROW(std::string("Failed to create command queue: ") + getCLError(err));
}
