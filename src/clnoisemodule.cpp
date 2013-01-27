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
#include "clnoise.h"

using namespace CLNoise;

Module::Module ( int attCount, int inpCount, int outCount, int contCount, const std::string mName, const char *kSource, Noise *ncl ) :
    inputCount ( inpCount ),
    controlCount ( contCount ),
    outputCount ( outCount ),
    attributeCount ( attCount ),
    moduleName ( mName ),
    attributes ( attCount ),
    inputs ( inpCount ),
    controls ( contCount ),
    kernelSource ( kSource ),
    moduleType ( BASE ),
    noiseCl ( ncl )
{
}

Module::~Module()
{
    attributes.clear();
    inputs.clear();
    controls.clear();
    kernelSource = 0;
}

Module &Module::operator= ( const Module& )
{
    return *this;
}

int Module::addControls ( Module *control )
{
    auto it = std::find_if ( controls.begin(), controls.end(), [&] ( Module * mod )
    {
        return mod == 0;
    } );
    if ( it != controls.end() )
    {
        *it = control;
        return it - controls.begin();
    }
    return -1;
}
int Module::addSource ( Module *source )
{
    auto it = std::find_if ( inputs.begin(), inputs.end(), [&] ( Module * mod )
    {
        return mod == 0;
    } );
    if ( it != inputs.end() )
    {
        *it = source;
        return it - inputs.begin();
    }
    return -1;
}
void Module::removeControl ( int id )
{
    if ( id >= controlCount ) THROW ( "Invalid index to remove" );
    controls[id] = 0;
}
void Module::removeSource ( int id )
{
    if ( id >= inputCount ) THROW ( "Invalid index to remove" );
    inputs[id] = 0;
}
void Module::setControls ( int id, Module *control )
{
    if ( id >= controlCount ) THROW ( "Invalid index to set" );
    controls[id] = control;
}
void Module::setSource ( int id, Module *source )
{
    if ( id >= inputCount ) THROW ( "Invalid index to set" );
    inputs[id] = source;
}
void Module::setAttribute ( const std::string &name, int value )
{
    auto it = std::find_if ( attributes.begin(), attributes.end(), [&] ( const ModuleAttribute & att )
    {
        return att.getName() == name;
    } );
    if ( it == attributes.end() ) THROW ( std::string ( "No attribute with name \"" ) + name + std::string ( "\" in module \"" ) + moduleName + "\"" );
    ( *it ).setValue ( value );
}
void Module::setAttribute ( const std::string &name, float value )
{
    auto it = std::find_if ( attributes.begin(), attributes.end(), [&] ( const ModuleAttribute & att )
    {
        return att.getName() == name;
    } );
    if ( it == attributes.end() ) THROW ( std::string ( "No attribute with name \"" ) + name + std::string ( "\" in module \"" ) + moduleName + "\"" );
    ( *it ).setValue ( value );
}

void Module::setAttribute ( int id, const ModuleAttribute &attribute )
{
    if ( id >= attributeCount ) THROW ( "Unable to set attribute. Too big index." );
    attributes[id] = attribute;
}

void Module::buildSource ( std::ostringstream &functionSet, std::ostringstream &kernelCode )
{
    for(Module *module : inputs)
    {
        if(!module)THROW ("Invalid input in module " + moduleName);
        module->buildSource(functionSet, kernelCode);
    }

    for(Module *control : controls)
    {
        if(!control)THROW ("Invalid control input in module " + moduleName);
        control->buildSource(functionSet, kernelCode);
    }

    functionSet<<kernelSource<<"\n";

    kernelCode<<"float "<<moduleName<<"Result = "<<moduleName<<"(pos, ";

    for(ModuleAttribute &att : attributes)
    {
        if(att.getName() != attributes.begin()->getName())kernelCode<<", ";

        if (att.getType() == ModuleAttribute::FLOAT)
        {
            kernelCode<<att.getFloat();
        }
        else
        {
            kernelCode<<att.getInt();
        }
    }
    kernelCode<<");\n";
}
