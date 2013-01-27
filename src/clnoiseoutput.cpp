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


#include "clnoiseoutput.h"
#include "clnoise.h"

using namespace CLNoise;

Output::Output ( int attCount, int inpCount, int contCount, const std::string mName, const char *kSource, Noise *ncl ) :
    Module ( attCount, inpCount, 0, contCount, mName, kSource, ncl ),
    width ( 0 ),
    height ( 0 ),
    clProgram ( 0 ),
    clKernel ( 0 ),
    output ( 0 ),
    isBuiled ( false ),
    isRunned ( false )
{
    moduleType = OUTPUT;
}

Output::~Output()
{
    freeResources();
}

void Output::freeResources()
{
    if ( output ) clReleaseMemObject ( output );
    if ( clProgram ) clReleaseProgram ( clProgram );
    if ( clKernel ) clReleaseKernel ( clKernel );
}

void Output::setImageDimension ( unsigned int w, unsigned int h )
{
    width = w;
    height = h;
}

void Output::build()
{
    std::ostringstream functionSet, kernelCode;

    kernelCode << "__kernel void " << moduleName << "(__write_only __global image2d_t output)\n";
    kernelCode << "{\n";
    kernelCode << "    int2 coord = (int2)(get_global_id(0), get_global_id(1));\n";
    kernelCode << "    int2 size = (int2)(get_global_size(0), get_global_size(1));\n";
    kernelCode << "    float2 pos = (float2)(coord.x / (float)size.x, coord.y / (float)size.y);\n";

    Module *input = *inputs.begin();
    if ( !input ) THROW ( "Output module don't have input!" );

    input->buildSource ( functionSet, kernelCode );

    kernelCode << "float input0 = " << input->moduleName << "Result;\n";
    kernelCode << kernelSource;
    kernelCode << "}\n";

    functionSet << kernelCode.str();
    buildedSource = functionSet.str();

    buildOpenCLKenel ( );
    isBuiled = true;
}

void Output::getImage ( unsigned char *buf )
{
    if ( !isRunned ) run();

    size_t origin[3] = {0, 0, 0};
    size_t region[3] = { ( size_t ) width, ( size_t ) height, 1};

    cl_int err = clEnqueueReadImage ( noiseCl->getCLCommandQueue(), output, CL_TRUE, origin, region, 0, 0, buf, 0, NULL, NULL );
    if ( err != CL_SUCCESS )
    {
        THROW ( std::string ( "Unable to read buffer!" ) + getCLError ( err ) );
    }
}

void Output::run()
{
    if ( !isBuiled ) build();

    size_t local[2] = {32, 32};
    size_t global[2] = { ( size_t ) width, ( size_t ) height};

    cl_int err = clEnqueueNDRangeKernel ( noiseCl->getCLCommandQueue(), clKernel, 2, NULL, global, local, 0, NULL, NULL );
    if ( err != CL_SUCCESS )
    {
        THROW ( std::string ( "Unable to enqueue kernel! " ) + getCLError ( err ) );
    }

    clFinish ( noiseCl->getCLCommandQueue() );
    isRunned = true;
}


void Output::buildOpenCLKenel ( )
{
    if ( isBuiled )
    {
        freeResources();
    }

    cl_int err;
    const char *src = buildedSource.c_str();
    clProgram = clCreateProgramWithSource ( noiseCl->getCLContext(), 1, &src, NULL, &err );
    if ( !clProgram ) THROW ( "Failed to create compute program!" );

    err = clBuildProgram ( clProgram, 0, NULL, NULL, NULL, NULL );
    if ( err != CL_SUCCESS )
    {
        size_t len;
        char buffer[2048];
        clGetProgramBuildInfo ( clProgram, noiseCl->getCLDevice(), CL_PROGRAM_BUILD_LOG, sizeof ( buffer ), buffer, &len );
        THROW ( std::string ( "Failed to build program:\n" ) + buffer );
    }

    clKernel = clCreateKernel ( clProgram, moduleName.c_str(), &err );
    if ( !clKernel || err != CL_SUCCESS )
    {
        if ( clKernel ) clReleaseKernel ( clKernel );
        THROW ( std::string ( "Failed to create compute kernel!" ) + getCLError ( err ) );
    }

    _cl_image_format format;
    format.image_channel_data_type = CL_UNSIGNED_INT8;
    format.image_channel_order = CL_RGBA;

    output = clCreateImage2D ( noiseCl->getCLContext(), CL_MEM_WRITE_ONLY, &format, width, height, 0, NULL, &err );
    if ( err != CL_SUCCESS ) THROW ( "Unable to create output image" );

    err = clSetKernelArg ( clKernel, 0, sizeof ( cl_mem ), &output );
    if ( err != CL_SUCCESS ) THROW ( "Unable to set kernel arg" );

}



