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

#include <algorithm>
#include "noisecl.h"
#include "noisemodule.h"
#include "noiseoutput.h"
#include "modules/modules_headers.h"

using namespace NOISECL;


NoiseCL::NoiseCL()
{
    init();

}

NoiseCL::~NoiseCL()
{

}

void NoiseCL::init()
{
#include "modules/modules.h"
}

NoiseModule *NoiseCL::createModule ( const std::string &name )
{
    auto it = availableModules.find(name);
    if (it == availableModules.end()) return 0;
    if (it->second->getModuleType() == NoiseModule::BASE) return new NoiseModule(*(it->second));
    return 0;
}

NoiseOutput *NoiseCL::createOutput ( const std::string &name )
{
    auto it = availableModules.find(name);
    if (it == availableModules.end()) return 0;
    if (it->second->getModuleType() == NoiseModule::OUTPUT) return new NoiseOutput(*(dynamic_cast<NoiseOutput*>(it->second)));
    return 0;
}
