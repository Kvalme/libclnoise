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
#include "modules/modules_headers.h"

using namespace CLNoise;


Noise::Noise()
{
    init();
    isCLAllocatedInternally = false;
}

Noise::~Noise()
{
    if ( isCLAllocatedInternally )
    {
        clReleaseCommandQueue ( clCommands );
        clReleaseContext ( clContext );
    }
}

void Noise::init()
{
#include "modules/modules.h"
}

std::vector<std::string> Noise::getModulesOfType(Module::MODULE_TYPE type)
{
    std::vector<std::string> modulesByType;
    for (auto &module : availableModules)
    {
        if (module.second->getModuleType() == type)modulesByType.push_back(module.first);
    }
    return modulesByType;
}

Module *Noise::createModule ( const std::string &name )
{
    auto it = availableModules.find ( name );
    if ( it == availableModules.end() ) return 0;
    if ( it->second->getModuleType() == Module::BASE ) return new Module ( * ( it->second ) );
    return 0;
}

Output *Noise::createOutput ( const std::string &name )
{
    auto it = availableModules.find ( name );
    if ( it == availableModules.end() ) return 0;
    if ( it->second->getModuleType() == Module::OUTPUT ) return new Output ( * ( dynamic_cast<Output *> ( it->second ) ) );
    return 0;
}

void Noise::initCLContext()
{
    // Connect to a compute device
    cl_int err;
    cl_platform_id clPlatform;
    err = clGetPlatformIDs ( 1, &clPlatform, 0 );
    if ( err != CL_SUCCESS ) THROW ( "Failed to find a platform!" );

    // Get a device of the appropriate type
    err = clGetDeviceIDs ( clPlatform, CL_DEVICE_TYPE_GPU , 1, &clDeviceId, 0 );
    if ( err != CL_SUCCESS ) THROW ( "Failed to create device group" );

    // Create a compute context
    clContext = clCreateContext ( 0, 1, &clDeviceId, NULL, NULL, &err );
    if ( !clContext ) THROW ( "Failed to create compute context!" );

    clCommands = clCreateCommandQueue ( clContext, clDeviceId, 0, &err );
    if ( !clCommands ) THROW ( "Failed to create command queue" );

}
