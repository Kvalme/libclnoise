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


#include "clnoiselibrary.h"
#include "clnoiseoutput.h"
#include "clnoisefunction.h"
#include "clnoisemodule.h"
#include "clnoisemodifier.h"


#include "modules/modules_headers.h"

using namespace CLNoise;

Library Library::__instance;
std::map<std::string, BaseModule *> Library::availableModules;

Library::Library()
{

}

Library::Library ( const Library & )
{

}

Library &Library::operator= ( const Library & )
{
    return *this;
}

Library::~Library()
{

}

void Library::init()
{
#include "modules/modules.h"
}

BaseModule *Library::createModule ( const std::string &name, BaseModule::MODULE_TYPE type )
{
    auto it = availableModules.find ( name );
    if ( it == availableModules.end() ) return nullptr;
    if ( it->second->getType() == type )
    {
        return it->second;
    }
    return nullptr;
}

std::vector< std::string > Library::getModulesOfType ( BaseModule::MODULE_TYPE type )
{
    std::vector<std::string> modulesByType;
for ( auto & module : availableModules )
    {
        if ( module.second->getType() == type ) modulesByType.push_back ( module.first );
    }
    return modulesByType;
}

BaseModule *Library::getModule ( const std::string &name )
{
    auto it = availableModules.find ( name );
    if ( it == availableModules.end() ) return nullptr;
    return it->second;
}

