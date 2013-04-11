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

#include "clnoise/attribute.h"

#include <stdio.h>
#include "clnoise/error.h"

using namespace CLNoise;

std::string Attribute::buildCode(int position) const
{
	char buf[2048];

	switch (type)
	{
		case Attribute::FLOAT:
			snprintf(buf, 2048, "float %s = floatAtt[%d];\n", attributeName.c_str(), position);
			break;
		case Attribute::INT:
			snprintf(buf, 2048, "int %s = intAtt[%d];\n", attributeName.c_str(), position);
			break;
		default:
			CL_THROW("Invalid attribute type");
	}
	return buf;
}

Attribute::Attribute(const std::string &name, int val, int min, int max) :
	type(INT), attributeName(name)
{
	intValue[0] = val;
	intValue[1] = min;
	intValue[2] = max;
	floatValue[0] = floatValue[1] = floatValue[2] = -1.0f;
};
Attribute::Attribute(const std::string &name, float val, int min, int max) :
	type(FLOAT), attributeName(name)
{
	floatValue[0] = val;
	floatValue[1] = min;
	floatValue[2] = max;
	intValue[0] = intValue[1] = intValue[2] = -1;
};

void Attribute::setValue(int val)
{
	type = INT;
	intValue[0] = val;
}

void Attribute::setValue(float val)
{
	type = FLOAT;
	floatValue[0] = val;
}
int Attribute::getIntMin() const
{
	if (type != INT) CL_THROW("Invalid type requested");
	return intValue[1];
}
int Attribute::getIntMax() const
{
	if (type != INT) CL_THROW("Invalid type requested");
	return intValue[2];
}
int Attribute::getInt() const
{
	if (type != INT) CL_THROW("Invalid type requested");
	return intValue[0];
}

float Attribute::getFloat() const
{
	if (type != FLOAT) CL_THROW("Invalid type requested");
	return floatValue[0];
}
float Attribute::getFloatMin() const
{
	if (type != FLOAT) CL_THROW("Invalid type requested");
	return floatValue[1];
}
float Attribute::getFloatMax() const
{
	if (type != FLOAT) CL_THROW("Invalid type requested");
	return floatValue[2];
}

Attribute::ATTRIBUTE_TYPE Attribute::getType() const
{
	return type;
}

const std::string &Attribute::getName() const
{
	return attributeName;
}

void Attribute::updateValue(unsigned int pos, float *floatAtt, int *intAtt, unsigned int floatAttSize, unsigned int intAttSize) const
{
	if ( type == FLOAT)
	{
		if (pos >= floatAttSize) CL_THROW("Attribute position is out of bounds");
		floatAtt[pos] = floatValue[0];
	}
	else if ( type == INT)
	{
		if (pos >= intAttSize) CL_THROW("Attribute position is out of bounds");
		intAtt[pos] = intValue[0];
	}
	else
	{
		CL_THROW("Invalid attribute type");
	}
}
