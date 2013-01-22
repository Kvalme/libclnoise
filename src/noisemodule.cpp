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

#include "noisemodule.h"

using namespace NOISECL;

NoiseModule::NoiseModule ( int attCount, int inpCount, int outCount, int contCount, const std::string mName, const char *kSource ) :
    inputCount ( inpCount ),
    controlCount ( contCount ),
    outputCount ( outCount ),
    attributeCount ( attCount ),
    moduleName ( mName ),
    attributes ( attCount ),
    inputs ( inpCount ),
    controls ( contCount ),
    kernelSource ( 0 )
{
}

NoiseModule::~NoiseModule()
{
    attributes.clear();
    inputs.clear();
    controls.clear();
    kernelSource = 0;
}

NoiseModule &NoiseModule::operator= ( const NoiseModule &other )
{
    return *this;
}

int NoiseModule::addControls ( NoiseModule *control )
{
    auto it = std::find_if ( controls.begin(), controls.end(), [&] ( NoiseModule * mod )
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
int NoiseModule::addSource ( NoiseModule *source )
{
    auto it = std::find_if ( inputs.begin(), inputs.end(), [&] ( NoiseModule * mod )
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
void NoiseModule::removeControl ( int id )
{
    if ( id >= controlCount ) THROW ( "Invalid index to remove" );
    controls[id] = 0;
}
void NoiseModule::removeSource ( int id )
{
    if ( id >= inputCount ) THROW ( "Invalid index to remove" );
    inputs[id] = 0;
}
void NoiseModule::setControls ( int id, NoiseModule *control )
{
    if ( id >= controlCount ) THROW ( "Invalid index to set" );
    controls[id] = control;
}
void NoiseModule::setSource ( int id, NoiseModule *source )
{
    if ( id >= inputCount ) THROW ( "Invalid index to set" );
    inputs[id] = source;
}
void NoiseModule::setAttribute ( const std::string &name, int value )
{
    auto it = std::find_if(attributes.begin(), attributes.end(), [&](const NoiseModuleAttribute &att){ return att.getName() == name; });
    if (it == attributes.end()) THROW(std::string("No attribute with name \"") + name + std::string("\" in module \"") + moduleName + "\"");
    (*it).setValue(value);
}
void NoiseModule::setAttribute ( const std::string &name, float value )
{
    auto it = std::find_if(attributes.begin(), attributes.end(), [&](const NoiseModuleAttribute &att){ return att.getName() == name; });
    if (it == attributes.end()) THROW(std::string("No attribute with name \"") + name + std::string("\" in module \"") + moduleName + "\"");
    (*it).setValue(value);
}

