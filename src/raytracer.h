
#pragma once

#include "src/rtlog.h"
#include "src/clobjects.h"
#include "src/raytracer/camera.h"
#include "src/raytracer/scene.h"


namespace rt {

enum class Format {
    RGBA8,  // 4x1= 4 bytes/pixel
    RGBA32F // 4x4=16 bytes/pixel
};


struct Config {
    cl_uint sampleCount;
    cl_uint bounceLimit;

    bool operator!=(const Config& other) {
        return (sampleCount != other.sampleCount) | (bounceLimit != other.bounceLimit);
    }
};


class Raytracer {

    public:
        Raytracer(glm::ivec2 imageShape, CL_Objects clObjects, Format format, bool allowAccumulation);
        void renderScene(const internal::Scene& scene, const internal::Camera& camera, const Config& config);
        void readPixels(void* outBuffer) const;
        bool saveAsImage(const char* filepath) const;
        void accumulatePixels();
        void resetFrameCount() { m_frameCount = 1; }

        bool isValid() const { return m_isValid; }
        Format getPixelFormat() const { return m_format; }
        bool allowsAccumulation() const { return m_allowAccumulation; }
        const glm::ivec2& getImageShape() const { return m_imageShape; }
        uint32_t getFrameCount() const { return m_frameCount; }
        uint32_t getPixelBufferSize() const;

    private:
        void createPixelBuffers();
        void createClKernels();
        std::string makeClProgramsBuildFlags() const;

    private:
        glm::ivec2 m_imageShape;
        CL_Objects m_clObjects;
        Format m_format;
        bool m_allowAccumulation;
        uint32_t m_frameCount = 1;
        bool m_isValid = true;
        Config m_lastConfig = {.sampleCount = 0};

        cl::Buffer m_pixelBuffer;
        cl::Buffer m_accumPixelBuffer;
        cl::Kernel m_raytracerKernel;
        cl::Kernel m_accumulatorKernel;

};

}
