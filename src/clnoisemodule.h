/*
    libnoisecl - procedural noise generation tool based on OpenCL library
    Copyright (C) 2013  Messenger of death <messengerofdeath@gmail.com>

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
#include <vector>
#include <sstream>
#include "clnoisemoduleattribute.h"

namespace CLNoise
{

class Noise;

class Module
{

public:

    enum MODULE_TYPE
    {
        BASE = 0,
        OUTPUT = 1,
    };

    Module ( int attCount, int inpCount, int outCount, int contCount, const std::string mName, const char *kSource, Noise *ncl );
    virtual ~Module();
    virtual Module &operator= ( const Module &other );

    int getInputCount() const
    {
        return inputCount;
    }
    int getControlCount() const
    {
        return controlCount;
    }
    int getOutputCount() const
    {
        return outputCount;
    }
    int getAttributeCount() const
    {
        return attributeCount;
    }
    const std::string &getName() const
    {
        return moduleName;
    }

    const std::vector<ModuleAttribute>& getAttributes ( int id ) const
    {
        return attributes;
    }
    void setAttribute ( const std::string &name, int value );
    void setAttribute ( const std::string &name, float value );

    void setSource ( int id, Module *source );
    void setControls ( int id, Module *control );

    int addSource ( Module *source );
    int addControls ( Module *control );

    void removeSource ( int id );
    void removeControl ( int id );

    const char *getKernelSource() const
    {
        return kernelSource;
    }

    MODULE_TYPE getModuleType() const
    {
        return moduleType;
    }

protected:
    void setAttribute(int id, const ModuleAttribute &attribute);
    void buildSource(std::ostringstream &functionSet, std::ostringstream &kernelCode);
    
    friend class Noise;
    friend class Output;
    
    int inputCount;
    int controlCount;
    int outputCount;
    int attributeCount;
    std::string moduleName;

    std::vector<ModuleAttribute> attributes;
    std::vector<Module *> inputs;
    std::vector<Module *> controls;

    const char *kernelSource;
    MODULE_TYPE moduleType;
    Noise *noiseCl;
};
}
