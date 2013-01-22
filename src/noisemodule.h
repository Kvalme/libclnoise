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


#ifndef NOISEMODULE_H
#define NOISEMODULE_H
#include <string>
#include <vector>
#include "noisemoduleattribute.h"

namespace NOISECL
{

class NoiseModule
{

public:
    NoiseModule ( int attCount, int inpCount, int outCount, int contCount, const std::string mName, const char *kSource );
    virtual ~NoiseModule();
    virtual NoiseModule &operator= ( const NoiseModule &other );

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

    const std::vector<NoiseModuleAttribute>& getAttributes ( int id ) const
    {
        return attributes;
    }
    void setAttribute ( const std::string &name, int value );
    void setAttribute ( const std::string &name, float value );

    void setSource ( int id, NoiseModule *source );
    void setControls ( int id, NoiseModule *control );

    int addSource ( NoiseModule *source );
    int addControls ( NoiseModule *control );

    void removeSource ( int id );
    void removeControl ( int id );

    const char *getKernelSource() const
    {
        return kernelSource;
    }

private:
    int inputCount;
    int controlCount;
    int outputCount;
    int attributeCount;
    std::string moduleName;

    std::vector<NoiseModuleAttribute> attributes;
    std::vector<NoiseModule *> inputs;
    std::vector<NoiseModule *> controls;

    const char *kernelSource;
};
}
#endif // NOISEMODULE_H
