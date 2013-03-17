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


#include "clnoise/output.h"
#include "clnoise/noise.h"
#include "clnoise/error.h"
#include "noisemap.h"

using namespace CLNoise;

Output::Output ( const std::string mName ) :
    BaseModule( mName, OUTPUT),
    width ( 0 ),
    height ( 0 ),
    data (nullptr)
{
    moduleType = OUTPUT;
}

Output::~Output()
{
    if(data)delete[] data;
}

void Output::setData (unsigned char *dat)
{
	if(data) delete[] data;
	data = dat;
}

void Output::getImageDimension (unsigned int *w, unsigned int *h) const
{
	*w = width;
	*h = height;
}

void Output::setImageDimension ( unsigned int w, unsigned int h )
{
    width = w;
    height = h;
}

void Output::buildHeader(NoiseMap*)
{
}

void Output::buildSource(NoiseMap *map)
{
	std::string source;
	
	for(unsigned a = 0; a < inputs.size(); ++a)
	{
		ContactInfo &c = inputs[a];
		if (c.input == nullptr) CL_THROW("NULL provided as input");
		std::string moduleName = c.input->getModuleCallName();
		
		source.append("#define ");
		source.append(c.name);
		source.append(" ");
		source.append(moduleName);
		source.append("(pos, intAttr, floatAttr)\n");
	}
	
	source.append(kernelSource);
	
	map->addKernelCode(source);
}
