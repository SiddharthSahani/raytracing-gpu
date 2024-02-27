
#pragma once

#include "src/rtlog.h"
#include "src/clutils.h"
#include "src/raytracer/camera.h"
#include "src/raytracer/scene.h"
#include <map>


namespace rt {

enum class Format {
    RGBA8,  // 4x1= 4 bytes/pixel
    RGBA32F // 4x4=16 bytes/pixel
};


struct Config {
    cl_uint sampleCount;
    cl_uint bounceLimit;
};

bool operator<(const Config& a, const Config& b) {
    return (a.sampleCount < b.sampleCount) || (a.bounceLimit < b.bounceLimit);
}


class Raytracer {

    public:
        Raytracer(glm::ivec2 imageShape, CL_Objects clObjects, Format format, bool allowAccumulation);
        void renderScene(const internal::Scene& scene, const internal::Camera& camera, const Config& config);
        void readPixels(void* outBuffer) const;
        bool saveAsImage(const char* filepath) const;
        void accumulatePixels();
        void resetFrameCount() { m_frameCount = 1; }

        const CL_Objects& getCl() const { return m_clObjects; }
        bool isValid() const { return m_isValid; }
        Format getPixelFormat() const { return m_format; }
        bool allowsAccumulation() const { return m_allowAccumulation; }
        const glm::ivec2& getImageShape() const { return m_imageShape; }
        uint32_t getFrameCount() const { return m_frameCount; }
        uint32_t getPixelBufferSize() const;
        void createClKernels(const rt::Config& config);

    private:
        void createPixelBuffers();
        std::string makeClProgramsBuildFlags(const rt::Config& config) const;

    private:
        glm::ivec2 m_imageShape;
        CL_Objects m_clObjects;
        Format m_format;
        bool m_allowAccumulation;
        uint32_t m_frameCount = 1;
        bool m_isValid = true;

        std::map<Config, cl::Kernel> m_kernels;

        cl::Image2D m_frameImage;
        cl::Image2D m_accumImage;
        cl::Kernel m_accumulatorKernel;

};

}
