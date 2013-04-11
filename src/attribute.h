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

namespace CLNoise
{
class Attribute
{
public:
	enum ATTRIBUTE_TYPE
	{
	        INVALID = -1,
	        INT     = 0,
	        FLOAT   = 1,
	        GRADIENT = 2
	};

	Attribute() : type(INVALID) {};
	explicit Attribute(const std::string &name, int val, int min, int max);
	explicit Attribute(const std::string &name, float val, int min, int max);
	virtual ~Attribute() {};

	void setValue(int val);
	void setValue(float val);

	int getIntMin() const;
	int getIntMax() const;
	int getInt() const;

	float getFloat() const;
	float getFloatMin() const;
	float getFloatMax() const;

	virtual ATTRIBUTE_TYPE getType() const;

	virtual const std::string &getName() const;

	virtual std::string buildCode(int position) const;


protected:
	friend class NoiseMap;
	virtual void updateValue(unsigned int pos, float *floatAtt, int *intAtt, unsigned int floatAttSize, unsigned int intAttSize) const;

	int intValue[3];
	float floatValue[3];
	ATTRIBUTE_TYPE type;
	std::string attributeName;
};
}
