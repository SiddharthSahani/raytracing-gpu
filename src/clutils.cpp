
#include "src/clutils.h"
// required for creating cl context
#define  GLFW_EXPOSE_NATIVE_WGL
#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>


namespace rt {

std::vector<cl::Platform> getAllClPlatforms() {
    std::vector<cl::Platform> res;
    cl::Platform::get(&res);
    return res;
}


std::vector<cl::Device> getAllClDevices(cl::Platform platform) {
    std::vector<cl::Device> res;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &res);
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


bool supports_clGlInterop(cl::Device device) {
    std::string allExtensions = device.getInfo<CL_DEVICE_EXTENSIONS>();
    const char* ext = "cl_khr_gl_sharing";
    return allExtensions.find(ext) != std::string::npos;
}


CL_Objects createClObjects_withInterop(cl::Platform platform, cl::Device device) {
    cl_context_properties props[] = {
        CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(),
        CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(),
        CL_CONTEXT_PLATFORM, (cl_context_properties) platform(),
        0
    };

    CL_Objects res;
    res.platform = platform;
    res.device = device;
    res.context = cl::Context(device, props);
    res.queue = cl::CommandQueue(res.context, res.device);
    return res;
}

}
