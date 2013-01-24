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


#ifndef NOISEOUTPUT_H
#define NOISEOUTPUT_H

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <CL/opencl.h>

#include "noisemodule.h"

namespace NOISECL
{

class NoiseCL;
class NoiseOutput : public NoiseModule
{

public:
    NoiseOutput ( int attCount, int inpCount, int contCount, const std::string mName, const char *kSource, NoiseCL *ncl );
    virtual ~NoiseOutput();
    void setImageDimension ( unsigned int w, unsigned int h );
    
    void getImage ( unsigned char *buf );
    void build();
    void run();

    std::string getBuildedSource() const { return buildedSource;}

private:
    void buildOpenCLKenel ( );

    unsigned int width;
    unsigned int height;

    cl_program clProgram;
    cl_kernel clKernel;

    cl_mem output;
    bool isBuiled;
    bool isRunned;

    std::string buildedSource;
    
};
}

#endif // NOISEOUTPUT_H
