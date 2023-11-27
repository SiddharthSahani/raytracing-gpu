
#pragma once

#include "src/cl_utils.h"

#define RT_LOG(text, ...)


namespace rt {

// modes: once? , realtime? , video?
// formats: R32G32B32A32? , R8G8B8?

struct Camera {};
struct Scene {};
struct Config {
    bool operator!=(const Config& other) const;
};


struct CL_Objects {
    cl::Device device;
    cl::Context context;
    cl::CommandQueue queue;
    cl::Kernel kernel;
};


bool chooseClDevice(cl::Device& device);
std::string getClProgramSource();
std::string makeProgramBuildFlags(const Config& config);

Config DEFAULT_CONFIG;


class Raytracer {

    public:
        Raytracer();
        void renderScene(const Scene& scene, const Camera& camera, const Config& config);
        void readPixels(void* out) const;

    private:
        void initializeClMembers();
        void makeClKernel(const Config& config);
        void createPixelBuffer();

    private:
        uint32_t m_viewportWidth, m_viewportHeight;
        CL_Objects m_cl;
        cl::Buffer m_pixelBuffer;
        Config m_lastConfig; // to not rebuild the cl program again

};


Raytracer::Raytracer() {
    initializeClMembers();
    makeClKernel(DEFAULT_CONFIG);
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


void Raytracer::makeClKernel(const Config& config) {
    std::string source = getClProgramSource();
    cl::Program program(source);

    std::string buildFlags = makeProgramBuildFlags(config);

    if (program.build(buildFlags.c_str())) {
        RT_LOG("Error while building Cl program\n");
        RT_LOG("Build Log:\n%s\n", program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_cl.device).c_str());
    }

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


void Raytracer::renderScene(const Scene& scene, const Camera& camera, const Config& config) {
    if (config != m_lastConfig) {
        m_lastConfig = config;
        makeClKernel(config);
    }

    m_cl.kernel.setArg(0, sizeof(Camera), &camera);
    m_cl.kernel.setArg(1, sizeof(Scene), &scene);
    m_cl.kernel.setArg(2, m_pixelBuffer);

    m_cl.queue.enqueueNDRangeKernel(
        m_cl.kernel,
        cl::NullRange,
        cl::NDRange(m_viewportWidth * m_viewportHeight),
        cl::NullRange
    );

    m_cl.queue.finish();
}


void Raytracer::readPixels(void* out) const {
    uint32_t bufferSize = m_viewportWidth * m_viewportHeight * sizeof(float) * 4;
    m_cl.queue.enqueueReadBuffer(m_pixelBuffer, true, 0, bufferSize, out);
}

}
