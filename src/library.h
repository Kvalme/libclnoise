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


#pragma once

#include <map>
#include <string>

#include "clnoise/basemodule.h"

namespace CLNoise
{

class Library
{
public:
    static Library &getInstance()
    {
        if ( availableModules.empty() ) __instance.init();
        return __instance;
    }

    BaseModule *createModule ( const std::string &name, BaseModule::MODULE_TYPE type );
    std::vector<std::string> getModulesOfType(BaseModule::MODULE_TYPE type);

    BaseModule *getModule(const std::string &name);

private:
    Library();
    Library ( const Library &other );
    ~Library();
    virtual Library &operator= ( const Library &other );
    void init();

    static std::map<std::string, BaseModule *> availableModules;

    static Library __instance;
};
}
