
#pragma once

#include "src/cl_utils.h"

#define RT_LOG(text, ...)


namespace rt {

// modes: once? , realtime? , video?
// formats: R32G32B32A32? , R8G8B8?

struct Camera {};
struct Scene {};
struct Config {};


struct CL_Objects {
    cl::Device device;
    cl::Context context;
    cl::CommandQueue queue;
    cl::Kernel kernel;
};


bool chooseClDevice(cl::Device& device);
bool buildClProgram(cl::Program& program);
std::string getClProgramSource();


class Raytracer {

    public:
        Raytracer();
        void renderScene(const Scene& scene, const Camera& camera, const Config& config);
        void readPixels(void* out) const;

    private:
        void initializeClMembers();
        void makeClKernel();
        void createPixelBuffer();

    private:
        uint32_t m_viewportWidth, m_viewportHeight;
        CL_Objects m_cl;
        cl::Buffer m_pixelBuffer;

};


Raytracer::Raytracer() {
    initializeClMembers();
    makeClKernel();
    createPixelBuffer();
}

void Raytracer::initializeClMembers() {
    if (!chooseClDevice(m_cl.device)) {
        RT_LOG("Unable to choose Cl Device\n");
        return;
    }

    m_cl.context = cl::Context(m_cl.device);
    m_cl.queue = cl::CommandQueue(m_cl.context, m_cl.device);
}

void Raytracer::makeClKernel() {
    std::string source;
    cl::Program program(source);

    program.build();

    m_cl.kernel = cl::Kernel(program, "renderScene");
}

void Raytracer::createPixelBuffer() {
    int err;

    uint32_t bufferSize = m_viewportWidth * m_viewportHeight * sizeof(float) * 4;
    m_pixelBuffer = cl::Buffer(m_cl.context, CL_MEM_READ_ONLY, bufferSize, nullptr, &err);

    if (err) {
        RT_LOG("Unable to allocate buffer of size %.3f MB\n", (float) bufferSize / (1024*1024));
    }
}



}
