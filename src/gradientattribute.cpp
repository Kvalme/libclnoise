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

#include "clnoise/gradientattribute.h"
#include "clnoise/error.h"

CLNoise::GradientAttribute::GradientAttribute(const std::string &name) :
	Attribute()
{
	type = GRADIENT;
}

void CLNoise::GradientAttribute::addPoint(float pos, CLNoise::GradientAttribute::GradientPoint point)
{
	auto it = points.find(pos);
	if (it != points.end())
	{
		it->second = point;
	}

	points.insert(std::make_pair(pos, point));
}

bool CLNoise::GradientAttribute::getPoint(int id, float *pos, CLNoise::GradientAttribute::GradientPoint *point) const
{
	auto it = points.begin();
	for (int a = 0; a < id && it != points.end(); ++a, ++it);
	if (it == points.end())return false;

	*pos = it->first;
	*point = it->second;
	return true;
}

int CLNoise::GradientAttribute::getPointCount() const
{
	return points.size();
}

void CLNoise::GradientAttribute::updateValue(unsigned int pos, float *floatAtt, int *intAtt, unsigned int floatAttSize, unsigned int intAttSize) const
{
	unsigned int maxPos = pos + MAX_POINTS_PER_GRADIENT * 5 + 1; //POS, R, G, B, A
	if (maxPos >= floatAttSize) CL_THROW("Position for GradientAttribute is out of bounds");
	if (points.size() > MAX_POINTS_PER_GRADIENT) CL_THROW("Too many points in gradient");

	floatAtt[pos++] = points.size();
	for (auto it = points.begin(); it != points.end(); ++it, pos ++)
	{
		floatAtt[pos] = it->first;
	}

	for (auto it = points.begin(); it != points.end(); ++it, pos ++)
	{
		floatAtt[pos] = it->second.r;
	}
	for (auto it = points.begin(); it != points.end(); ++it, pos ++)
	{
		floatAtt[pos] = it->second.g;
	}
	for (auto it = points.begin(); it != points.end(); ++it, pos ++)
	{
		floatAtt[pos] = it->second.b;
	}
	for (auto it = points.begin(); it != points.end(); ++it, pos ++)
	{
		floatAtt[pos] = it->second.a;
	}

}

std::string CLNoise::GradientAttribute::buildCode(int position) const
{
	char buf[2048];


	snprintf(buf, 2048, "int GRAD_POINT_COUNT = floatAtt[%d];\n"
	         "__global __read_only float *GRAD_POINT = floatAtt+%d;\n"
		 "__global __read_only float *GRAD_POINT_COLOR_R = floatAtt+%lu;\n"
		 "__global __read_only float *GRAD_POINT_COLOR_G = floatAtt+%lu;\n"
		 "__global __read_only float *GRAD_POINT_COLOR_B = floatAtt+%lu;\n"
		 "__global __read_only float *GRAD_POINT_COLOR_A = floatAtt+%lu;\n",
	         position,
	         position + 1,
	         position + 1 + points.size(),
	         position + 1 + points.size() + points.size(),
	         position + 1 + points.size() + points.size() + points.size(),
	         position + 1 + points.size() + points.size() + points.size() + points.size());
	
	return buf;
}
