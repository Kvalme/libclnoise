#include <iostream>
#include <sys/time.h>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "libnoise/noise.h"

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include "CL/opencl.h"

class Time
{
public:
    Time();
    ~Time();
    double get_elapsed_time();
    double get_elapsed_time_noreset();
    void reset();
private:
    uint64_t _last_access_time_;
};

Time::Time()
{
    timeval tv;
    gettimeofday ( &tv, 0 );
    _last_access_time_ = tv.tv_sec * 1000000 + tv.tv_usec;
}
Time::~Time()
{
}
double Time::get_elapsed_time( )
{
    timeval tv;
    gettimeofday ( &tv, 0 );
    uint64_t dtime, cur_time;
    cur_time = tv.tv_sec * 1000000 + tv.tv_usec;
    dtime = cur_time - _last_access_time_;
    _last_access_time_ = cur_time;
    return ( double ) dtime / 1000000.;
}
double Time::get_elapsed_time_noreset( )
{
    timeval tv;
    gettimeofday ( &tv, 0 );
    uint64_t dtime, cur_time;
    cur_time = tv.tv_sec * 1000000 + tv.tv_usec;
    dtime = cur_time - _last_access_time_;
    return ( double ) dtime / 1000000.;
}
void Time::reset()
{
    timeval tv;
    gettimeofday ( &tv, 0 );
    _last_access_time_ = tv.tv_sec * 1000000 + tv.tv_usec;
}

char *readKernelSource()
{
    char *srcBuf = 0;

    int file = open ( "testkernel.cl", O_RDONLY );
    if ( file < 0 ) throw std::runtime_error ( "Unable to open OpenCL kernel source." );

    struct stat statBuf;
    if ( fstat ( file, &statBuf ) != 0 ) throw std::runtime_error ( "Unable to get OpenCL kernel source size." );
    srcBuf = new char[statBuf.st_size];
    if ( !srcBuf ) throw std::runtime_error ( "Unable to allocate memory for OpenCL kernel source." );

    int rs = read(file, srcBuf, statBuf.st_size);
    if (rs != statBuf.st_size) throw std::runtime_error ( "Unable to read OpenCL kernel source." );
    return srcBuf;
}
void runOpenCLKernel ( int w, int h )
{
    Time openClTimer;
    Time openClTotalTimer;

    const char *kernelSource = readKernelSource();

    cl_device_id clDevice[10];
    cl_platform_id clPlatform;

    cl_int err;

    cl_uint platformCount;

    // Connect to a compute device
    err = clGetPlatformIDs ( 1, &clPlatform, &platformCount );
    if ( err != CL_SUCCESS ) throw ( std::runtime_error ( "Failed to find a platform!" ) );
    std::cout<<"Found "<<platformCount<<" OpenCL platforms."<<std::endl;

    // Get a device of the appropriate type
    cl_uint deviceCount;
    err = clGetDeviceIDs ( clPlatform, CL_DEVICE_TYPE_ALL , 10, clDevice, &deviceCount );
    if ( err != CL_SUCCESS ) throw ( std::runtime_error ( "Failed to create device group" ) );

    std::cout << "Found " << deviceCount << " OpenCL devices. Running benchmark." << std::endl;
    for ( unsigned int devId = 0; devId < deviceCount; ++devId )
    {
        char deviceName[512];
        size_t nameSize;
        err = clGetDeviceInfo ( clDevice[devId], CL_DEVICE_NAME, 512, deviceName, &nameSize );
        if ( err != CL_SUCCESS ) throw ( std::runtime_error ( "Failed to get device name" ) );
        std::cout << "Selected OpenCL device: \"" << deviceName << "\"" << std::endl;

        cl_command_queue commands = 0; // compute command queue
        cl_program program = 0;        // compute program
        cl_kernel kernel = 0;          // compute kernel
        cl_int err = 0;
        cl_mem outputImage = 0;
        cl_context clContext;
        cl_device_id currentDevice = clDevice[devId];

        openClTimer.reset();
        openClTotalTimer.reset();

        // Create a compute context
        clContext = clCreateContext ( 0, 1, &currentDevice, NULL, NULL, &err );
        if ( !clContext ) throw ( std::runtime_error ( "Failed to create compute context!" ) );

        commands = clCreateCommandQueue ( clContext, currentDevice, 0, &err );
        if ( !commands ) throw ( std::runtime_error ( "Failed to create command queue" ) );

        program = clCreateProgramWithSource ( clContext, 1, &kernelSource, NULL, &err );
        if ( !program ) throw ( std::runtime_error ( "Failed to create compute program!" ) );

        err = clBuildProgram ( program, 0, NULL, NULL, NULL, NULL );
        if ( err != CL_SUCCESS )
        {
            size_t len;
            char buffer[2048];
            clGetProgramBuildInfo ( program, currentDevice, CL_PROGRAM_BUILD_LOG,
                                    sizeof ( buffer ), buffer, &len );
            throw std::runtime_error(std::string("Failed to build program executable\n") + buffer);

        }

        kernel = clCreateKernel ( program, "testKernel", &err );
        if ( !kernel || err != CL_SUCCESS )throw std::runtime_error ( "Failed to create compute kernel!" );

        _cl_image_format format;
        format.image_channel_data_type = CL_UNSIGNED_INT8;
        format.image_channel_order = CL_RGBA;

        outputImage = clCreateImage2D ( clContext, CL_MEM_WRITE_ONLY, &format, w, h, 0, NULL, &err );
        if ( err != CL_SUCCESS ) throw std::runtime_error ( "Unable to create output image" );

        err = clSetKernelArg ( kernel, 0, sizeof ( cl_mem ), &outputImage );
        if ( err != CL_SUCCESS ) throw std::runtime_error ( "Unable to set kernel arg" );

        double timeElapsed = openClTimer.get_elapsed_time();
        std::cout<<"OpenCL context creation and kernel build: "<<timeElapsed<<"s ("<<timeElapsed * 1000000.<<"us)"<<std::endl;

        size_t local[2] = {64, 1};
        size_t global[2] = {(size_t)w, (size_t)h};

        err = clEnqueueNDRangeKernel ( commands, kernel, 2, NULL, global, local, 0, NULL, NULL );
        if ( err != CL_SUCCESS )
        {
            throw std::runtime_error ( "Unable to enqueue kernel!" );
        }

        clFinish ( commands );
        
        timeElapsed = openClTimer.get_elapsed_time();
        std::cout<<"OpenCL kernel execution time: "<<timeElapsed<<"s ("<<timeElapsed * 1000000.<<"us)"<<std::endl;

        int *colors = new int[w * h];
        if(!colors)throw std::runtime_error("Unable to allocate output buffer");

        size_t origin[3] = {0, 0, 0};
        size_t region[3] = {(size_t)w, (size_t)h, 1};

        err = clEnqueueReadImage ( commands, outputImage,
                                   CL_TRUE, origin, region, 0, 0,
                                   ( void * ) colors, 0, NULL, NULL );

        if ( err != CL_SUCCESS )
        {
            
            std::cerr<<"Err id:"<<err<<std::endl;
            throw std::runtime_error ( "Unable to read buffer!" );
        }

        timeElapsed = openClTimer.get_elapsed_time();
        std::cout<<"OpenCL data transfer to host time: "<<timeElapsed<<"s ("<<timeElapsed * 1000000.<<"us)"<<std::endl;

        //cleanup
        clReleaseMemObject ( outputImage );
        clReleaseProgram ( program );
        clReleaseKernel ( kernel );
        clReleaseCommandQueue ( commands );
        clReleaseContext ( clContext );
        
        timeElapsed = openClTimer.get_elapsed_time();
        std::cout<<"OpenCL cleanup time: "<<timeElapsed<<"s ("<<timeElapsed * 1000000.<<"us)"<<std::endl;
        
        timeElapsed = openClTotalTimer.get_elapsed_time();
        std::cout<<"Total time on selected device: "<<timeElapsed<<"s ("<<timeElapsed * 1000000.<<"us)"<<std::endl;
    }
}

void runLibNoiseTest(int w, int h)
{

    Time libnoiseTimer;
    
    noise::module::Perlin perlin;
    perlin.SetFrequency(1.0);
    perlin.SetLacunarity(2.0);
    perlin.SetNoiseQuality(noise::QUALITY_BEST);
    perlin.SetOctaveCount(6);
    perlin.SetPersistence(0.5);
    perlin.SetSeed(0);

    int *colors = new int[w * h];

    for(int y = 0; y < h; ++y)
    {
        for(int x = 0; x < w; ++x)
        {
            double xd((double)x/(double)w);
            double yd((double)y/(double)h);
            
            double val = perlin.GetValue(xd, 1.0, yd);
            int color = val * 255.;
            colors[x+y*w] = color | color<<8 | color<<16 | color<<24;
        }
    }

    double timeElapsed = libnoiseTimer.get_elapsed_time();
    std::cout<<"Libnoise time: "<<timeElapsed<<"s ("<<timeElapsed * 1000000.<<"us)"<<std::endl;
    
}

int main ( int argc, char **argv )
{
    if ( argc < 3 )
    {
        std::cerr << "Usage:\n" << argv[0] << " width height" << std::endl;
        return 1;
    }
    int w ( atol ( argv[1] ) ), h ( atol ( argv[2] ) );

    try
    {
        runOpenCLKernel ( w, h );
        runLibNoiseTest (w, h);
    }
    catch ( std::runtime_error &err )
    {
        std::cerr << "Got error:\n" << err.what() << std::endl;
    }

    return 0;
}
