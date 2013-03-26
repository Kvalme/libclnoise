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
