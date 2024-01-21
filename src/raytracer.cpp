
#include "src/raytracer.h"
#include <stb/stb_image_write.h>
#include <sstream>
#include <fstream>


namespace rt {

std::string readFile(const char* filepath) {
    std::ifstream file(filepath, std::ios::in | std::ios::ate);

    int fileSize = file.tellg();
    std::string fileContents(fileSize, '\0');

    file.seekg(0);
    file.read(&fileContents[0], fileSize);
    return fileContents;
}


cl::ImageFormat getClImageFormat(Format format) {
    switch (format) {
        case Format::RGBA8:
            return cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);
        case Format::RGBA32F:
            return cl::ImageFormat(CL_RGBA, CL_FLOAT);
        default:
            RT_LOG("Not implemented in `getClImageFormat`");
            return cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);
    }
}


Raytracer::Raytracer(glm::ivec2 imageShape, CL_Objects clObjects, Format format, bool allowAccumulation) {
    m_imageShape = imageShape;
    m_clObjects = clObjects;
    m_format = format;
    m_allowAccumulation = allowAccumulation;
    createPixelBuffers();

    if (m_format == Format::RGBA8 && m_allowAccumulation) {
        RT_LOG("Note: Using RGBA8 format with accumulation gives bad results");
    }
}


void Raytracer::renderScene(const internal::Scene& scene, const internal::Camera& camera, const Config& config) {
    if (!m_isValid) {
        RT_LOG("Instance is not valid");
        return;
    }

    if (m_lastConfig != config) {
        m_lastConfig = config;
        createClKernels();
    }

    m_raytracerKernel.setArg(0, sizeof(internal::Camera), &camera);
    m_raytracerKernel.setArg(1, sizeof(internal::SceneExtra), &scene.extra);
    m_raytracerKernel.setArg(2, scene.objectsBuffer);
    m_raytracerKernel.setArg(3, scene.materialsBuffer);
    m_raytracerKernel.setArg(4, sizeof(uint32_t), &m_frameCount);
    m_raytracerKernel.setArg(5, m_frameImage);

    m_clObjects.queue.enqueueNDRangeKernel(
        m_raytracerKernel,
        cl::NullRange,
        cl::NDRange(m_imageShape.x * m_imageShape.y),
        cl::NullRange
    );
    m_clObjects.queue.finish();
}


void Raytracer::readPixels(void* outBuffer) const {
    uint32_t bufferSize = getPixelBufferSize();

    if (m_allowAccumulation) {
        m_clObjects.queue.enqueueReadImage(m_accumImage, true, {0, 0, 0}, {(size_t) m_imageShape.x, (size_t) m_imageShape.y, 1}, 0, 0, outBuffer);
    } else {
        m_clObjects.queue.enqueueReadImage(m_frameImage, true, {0, 0, 0}, {(size_t) m_imageShape.x, (size_t) m_imageShape.y, 1}, 0, 0, outBuffer);
    }
}


bool Raytracer::saveAsImage(const char* filepath) const {
    if (m_format == Format::RGBA32F) {
        RT_LOG("Does not work for this format as of now");
        return false;
    }

    uint8_t* imageBuffer = new uint8_t[getPixelBufferSize()];
    readPixels(imageBuffer);
    bool res = stbi_write_png(filepath, m_imageShape.x, m_imageShape.y, 4, imageBuffer, 0);
    delete[] imageBuffer;
    return res;
}


void Raytracer::accumulatePixels() {
    if (!m_isValid) {
        RT_LOG("Instance is not valid");
        return;
    }

    if (!m_allowAccumulation) {
        RT_LOG("Can not accumulate pixels, allow it from class constructor");
        return;
    }

    m_accumulatorKernel.setArg(0, m_frameImage);
    m_accumulatorKernel.setArg(1, m_accumImage);
    m_accumulatorKernel.setArg(2, sizeof(uint32_t), &m_frameCount);
    m_accumulatorKernel.setArg(3, sizeof(uint32_t), &m_imageShape.x);

    m_clObjects.queue.enqueueNDRangeKernel(
        m_accumulatorKernel,
        cl::NullRange,
        cl::NDRange(m_imageShape.x * m_imageShape.y),
        cl::NullRange
    );
    m_clObjects.queue.finish();

    m_frameCount++;
}


uint32_t Raytracer::getPixelBufferSize() const {
    uint32_t numPixels = m_imageShape.x * m_imageShape.y;
    switch (m_format) {
        case Format::RGBA8:
            return numPixels * 4 * sizeof(uint8_t);
        case Format::RGBA32F:
            return numPixels * 4 * sizeof(float);
        default:
            return 0;
    }
}


void Raytracer::createPixelBuffers() {
    int err = 0;
    uint32_t bufferSize = getPixelBufferSize();

    cl::ImageFormat imgFormat = getClImageFormat(m_format);
    m_frameImage = cl::Image2D(m_clObjects.context, CL_MEM_READ_WRITE, imgFormat, m_imageShape.x, m_imageShape.y, 0, nullptr, &err);
    if (m_allowAccumulation) {
        m_accumImage = cl::Image2D(m_clObjects.context, CL_MEM_READ_WRITE, imgFormat, m_imageShape.x, m_imageShape.y, 0, nullptr, &err);
    }

    uint32_t totalBufferSize = bufferSize * (m_allowAccumulation ? 2 : 1);

    if (err) {
        RT_LOG("Unable to allocate buffers of size %.3f MB", (float) totalBufferSize / (1024 * 1024));
        m_isValid = false;
    } else {
        RT_LOG("Allocated buffers of size %.3f MB", (float) totalBufferSize / (1024 * 1024));
    }
}


void Raytracer::createClKernels() {
    std::string raytracerFileSource = readFile("kernels/raytracer.cl");
    std::string accumulatorFileSource = readFile("kernels/accumulator.cl");

    if (raytracerFileSource.empty() || accumulatorFileSource.empty()) {
        RT_LOG("Something went wrong while reading the Cl files");
        return;
    }

    cl::Program raytracerProgram = cl::Program(raytracerFileSource);
    cl::Program accumulatorProgram = cl::Program(accumulatorFileSource);

    std::string buildFlags = makeClProgramsBuildFlags();
    RT_LOG("(Re)building Cl Programs with flags: %s", buildFlags.c_str());

    if (raytracerProgram.build(buildFlags.c_str()) || accumulatorProgram.build(buildFlags.c_str())) {
        RT_LOG("Encountered error while building Cl programs");
        std::string raytracerBuildLog = raytracerProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_clObjects.device);
        std::string accumulatorBuildLog = accumulatorProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_clObjects.device);
        RT_LOG("Build log for raytracer:\n%s", raytracerBuildLog.c_str());
        RT_LOG("--------------------------");
        RT_LOG("Build log for accumulator:\n%s", accumulatorBuildLog.c_str());
    } else {
        RT_LOG("Built Cl programs successfully");
        m_raytracerKernel = cl::Kernel(raytracerProgram, "raytraceScene");
        m_accumulatorKernel = cl::Kernel(accumulatorProgram, "accumulateFrameData");
    }
}


std::string Raytracer::makeClProgramsBuildFlags() const {
    std::stringstream stream;

    stream << " -cl-std=CL2.0";

    stream << " -DCONFIG__SAMPLE_COUNT=" << m_lastConfig.sampleCount;
    stream << " -DCONFIG__BOUNCE_LIMIT=" << m_lastConfig.bounceLimit;

    return stream.str();
}

}
