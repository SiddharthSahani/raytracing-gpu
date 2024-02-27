
#pragma once

#include <CL/opencl.hpp>


namespace rt {

struct CL_Objects {
    cl::Platform platform;
    cl::Device device;
    cl::Context context;
    cl::CommandQueue queue;
};


std::vector<cl::Platform> getAllClPlatforms();

// Only the GPU(s)
std::vector<cl::Device> getAllClDevices(cl::Platform platform);


// creates context normally
CL_Objects createClObjects(cl::Platform platform, cl::Device device);

}
