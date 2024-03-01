
#pragma once

#include <CL/cl.hpp>


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


// checks for cl-gl interop (memory sharing)
bool supports_clGlInterop(cl::Device device);


// creates the context with cl-gl interop enabled
// Note: Opengl should be initialized
CL_Objects createClObjects_withInterop(cl::Platform platform, cl::Device device);

}
