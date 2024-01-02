
#pragma once

#include <CL/opencl.hpp>


namespace rt {

struct CL_Objects {
    cl::Platform platform;
    cl::Device device;
    cl::Context context;
    cl::CommandQueue queue;
};


std::vector<cl::Platform> getAllClPlatforms() {
std::vector<cl::Platform> res;
    cl::Platform::get(&res);
    return res;
}


std::vector<cl::Device> getAllClDevices(cl::Platform platform) {
    std::vector<cl::Device> res;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &res);
    return res;
}


CL_Objects createClObjects(cl::Platform platform, cl::Device device) {
    CL_Objects res;
    res.platform = platform;
    res.device = device;
    res.context = cl::Context(device);
    res.queue = cl::CommandQueue(res.context, res.device);
    return res;
}

}
