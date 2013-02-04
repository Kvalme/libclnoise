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
#include "clnoisebasemodule.h"

namespace CLNoise
{

class Module : public BaseModule
{

public:


    Module ( int attCount, int inpCount, int outCount, int contCount, const std::string &mName, const char *kSource );
    virtual ~Module();

    int getInputCount() const
    {
        return m_inputCount;
    }
    int getControlCount() const
    {
        return m_controlCount;
    }
    int getOutputCount() const
    {
        return m_outputCount;
    }
    int getAttributeCount() const
    {
        return m_attributeCount;
    }

    const std::vector<ModuleAttribute>& getAttributes ( ) const
    {
        return m_attributes;
    }

    void setAttribute ( const std::string &name, int value );
    void setAttribute ( const std::string &name, float value );
    void setAttribute ( int id, const ModuleAttribute &attribute );

    void setInput ( int id, Module *source );
    void setControl ( int id, Module *control );

    int addInput ( Module *source );
    int addControl ( Module *control );

    void removeInput ( int id );
    void removeControl ( int id );

    const std::vector<Module *>& getInputs() const { return m_inputs;}
    const std::vector<Module *>& getControls() const { return m_controls;}
    const std::vector<ModuleAttribute>& getAttributes() const { return m_attributes;}

protected:

    int m_inputCount;
    int m_controlCount;
    int m_outputCount;
    int m_attributeCount;

    std::vector<ModuleAttribute> m_attributes;
    std::vector<Module *> m_inputs;
    std::vector<Module *> m_controls;

};
}
