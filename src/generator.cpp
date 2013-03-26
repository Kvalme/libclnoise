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
#include <stdio.h>
#include <string.h>
#include <sstream>

#include "clnoise/generator.h"
#include "clnoise/noisemap.h"
#include "clnoise/error.h"

using namespace CLNoise;

Generator::Generator(const std::string &mName) :
	BaseModule(mName, GENERATOR)
{
}

Generator::~Generator()
{
}


void Generator::buildHeader(NoiseMap *map)
{
	proto.clear();
	for (unsigned a = 0; a<attributes.size(); ++a)
	{
		map->addAttribute(this, attributes[a]);
	}
	
	switch(outputType)
	{
		case ContactInfo::FLOAT:
			proto.append("float ");
			break;
		case ContactInfo::RGBA:
			proto.append("int ");
			break;
		default:
			CL_THROW("Invalid attribute type");
	}
	
	proto.append(moduleName);
	proto.append("(float2 pos, __global __read_only int *intAtt, __global __read_only float *floatAtt)");
	
	map->addFunctionPrototype(proto);
}

void Generator::buildSource(NoiseMap *map)
{
	std::stringstream source;
	
	source<<proto<<"\n{\n";
	
	if (!attributes.empty())
	{
		auto attributeMap = map->getAttributeMap(this);
		
		for (unsigned a = 0; a<attributes.size(); ++a)
		{
			Attribute &att = attributes[a];
			auto amIt = attributeMap.find(att.getName());
			
			switch(att.getType())
			{
				case Attribute::FLOAT:
					source<<"float "<<att.getName()<<" = floatAtt["<<amIt->second<<"];\n";
					break;
				case Attribute::INT:
					source<<"int "<<att.getName()<<" = intAtt["<<amIt->second<<"];\n";
					break;
				default:
					CL_THROW("Invalid attribute type");
			}
		}
	}
	source<<kernelSource<<"}\n";
	
	map->addFunctionSource(source.str());
}
