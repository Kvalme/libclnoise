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

#pragma once


#include <string>
#include <map>
#include "clnoise/attribute.h"

namespace CLNoise
{
class GradientAttribute : public Attribute
{
public:
	struct GradientPoint
	{
		float r;
		float g;
		float b;
		float a;
		GradientPoint(float ri, float gi, float bi, float ai) : r(ri), g(gi), b(bi), a(ai){}
	};
	
	GradientAttribute(const std::string &name);
	
	void addPoint(float pos, GradientPoint point);


private:
	ATTRIBUTE_TYPE type;
	std::map<float, GradientPoint> points;
};
}
