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

#include "clnoisemodule.h"

namespace CLNoise
{

class NoiseMap;


class Output : public Module
{
public:
	Output (int attCount, int inpCount, int contCount, const std::string mName, const char *kSource);
	virtual ~Output();

	void setImageDimension (unsigned int w, unsigned int h);
	void getImageDimension (unsigned int *w, unsigned int *h) const;
	unsigned char* getData () { return data;}

private:

	friend class NoiseMap;
	void setData(unsigned char *dat);

	unsigned int width;
	unsigned int height;

	unsigned char *data;
};
}
