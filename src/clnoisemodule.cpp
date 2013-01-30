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


#include <algorithm>
#include "clnoisemodule.h"


using namespace CLNoise;

Module::Module ( int attCount, int inpCount, int outCount, int contCount, const std::string &mName, const char *kSource ) :
    BaseModule ( mName, kSource ),
    m_inputCount ( inpCount ),
    m_controlCount ( contCount ),
    m_outputCount ( outCount ),
    m_attributeCount ( attCount ),
    m_attributes ( attCount ),
    m_inputs ( inpCount ),
    m_controls ( contCount )
{
    m_moduleType = BASE;
}

Module::~Module()
{
    m_attributes.clear();
    m_inputs.clear();
    m_controls.clear();
}

int Module::addControl ( Module *control )
{
    auto it = std::find_if ( m_controls.begin(), m_controls.end(), [&] ( Module * mod )
    {
        return mod == 0;
    } );
    if ( it != m_controls.end() )
    {
        *it = control;
        return it - m_controls.begin();
    }
    return -1;
}
int Module::addInput ( Module *source )
{
    auto it = std::find_if ( m_inputs.begin(), m_inputs.end(), [&] ( Module * mod )
    {
        return mod == 0;
    } );
    if ( it != m_inputs.end() )
    {
        *it = source;
        return it - m_inputs.begin();
    }
    return -1;
}
void Module::removeControl ( int id )
{
    if ( id >= m_controlCount ) THROW ( "Invalid index to remove" );
    m_controls[id] = 0;
}
void Module::removeInput ( int id )
{
    if ( id >= m_inputCount ) THROW ( "Invalid index to remove" );
    m_inputs[id] = 0;
}
void Module::setControl ( int id, Module *control )
{
    if ( id >= m_controlCount ) THROW ( "Invalid index to set" );
    m_controls[id] = control;
}
void Module::setInput ( int id, Module *source )
{
    if ( id >= m_inputCount ) THROW ( "Invalid index to set" );
    m_inputs[id] = source;
}
void Module::setAttribute ( const std::string &name, int value )
{
    auto it = std::find_if ( m_attributes.begin(), m_attributes.end(), [&] ( const ModuleAttribute & att )
    {
        return att.getName() == name;
    } );
    if ( it == m_attributes.end() ) THROW ( std::string ( "No attribute with name \"" ) + name + std::string ( "\" in module \"" ) + m_moduleName + "\"" );
    ( *it ).setValue ( value );
}
void Module::setAttribute ( const std::string &name, float value )
{
    auto it = std::find_if ( m_attributes.begin(), m_attributes.end(), [&] ( const ModuleAttribute & att )
    {
        return att.getName() == name;
    } );
    if ( it == m_attributes.end() ) THROW ( std::string ( "No attribute with name \"" ) + name + std::string ( "\" in module \"" ) + m_moduleName + "\"" );
    ( *it ).setValue ( value );
}

void Module::setAttribute ( int id, const ModuleAttribute &attribute )
{
    if ( id >= m_attributeCount ) THROW ( "Unable to set attribute. Too big index." );
    m_attributes[id] = attribute;
}

/*void Module::buildSource ( std::ostringstream &functionSet, std::ostringstream &kernelCode )
{
for ( Module * module : inputs )
    {
        if ( !module ) THROW ( "Invalid input in module " + moduleName );
        module->buildSource ( functionSet, kernelCode );
    }

for ( Module * control : controls )
    {
        if ( !control ) THROW ( "Invalid control input in module " + moduleName );
        control->buildSource ( functionSet, kernelCode );
    }

    functionSet << kernelSource << "\n";

    kernelCode << "float " << moduleName << "Result = " << moduleName << "(pos, ";

for ( ModuleAttribute & att : attributes )
    {
        if ( att.getName() != attributes.begin()->getName() ) kernelCode << ", ";

        if ( att.getType() == ModuleAttribute::FLOAT )
        {
            kernelCode << att.getFloat();
        }
        else
        {
            kernelCode << att.getInt();
        }
    }
    kernelCode << ");\n";
}*/
