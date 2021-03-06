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


#include "library.h"

#include "clnoise/output.h"
#include "clnoise/function.h"
#include "clnoise/basemodule.h"
#include "clnoise/filter.h"
#include "clnoise/generator.h"

#include "clnoise/gradientattribute.h"

#include "modules/modules_headers.h"

using namespace CLNoise;

std::map<std::string, BaseModule *> Library::availableModules;
Library Library::__instance;

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
	for (auto it = availableModules.begin(); it != availableModules.end(); ++it)
	{
		if (it->second)
		{
			delete it->second;
		}
		it->second = nullptr;
	}
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

