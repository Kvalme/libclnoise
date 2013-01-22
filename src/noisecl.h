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

#pragma once
#include <string>
#include <map>
#include <CL/opencl.h>

namespace NOISECL
{

class NoiseOutput;
class NoiseModule;
class NoiseCL
{
public:
    NoiseCL();
    ~NoiseCL();

    NoiseModule* createModule(const std::string &name);
    NoiseOutput* createOutput(const std::string &name);
    void initCLContext();
    
private:
    friend class NoiseOutput;
    void init();
    std::map<std::string, NoiseModule*> availableModules;

    cl_device_id getCLDevice()
    {
        return clDeviceId;
    }
    cl_context getCLContext()
    {
        return clContext;
    }
    cl_command_queue getCLCommandQueue()
    {
        return clCommands;
    }
    
    cl_device_id clDeviceId;
    cl_context clContext;
    cl_command_queue clCommands;
    bool isCLAllocatedInternally;
};

}
