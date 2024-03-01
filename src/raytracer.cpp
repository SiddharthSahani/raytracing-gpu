
#include "src/raytracer.h"
#include <stb/stb_image_write.h>
#include <sstream>
#include <fstream>


namespace rt {

std::string readFile(const char* filepath) {
    std::ifstream file(filepath, std::ios::in | std::ios::ate);

    int fileSize = file.tellg();
    if (fileSize == -1) {
        // file doesnt exist
        printf("ERROR (`readFile`): Unable to read %s\n", filepath);
        fileSize = 0;
    }
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
        case Format::RGBA16F:
            return cl::ImageFormat(CL_RGBA, CL_HALF_FLOAT);
        default:
            printf("ERROR (`getClImageFormat`): Not implemented\n");
            return cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);
    }
}


Raytracer::Raytracer(glm::ivec2 imageShape, CL_Objects clObjects, Format format, bool allowAccumulation, uint32_t glTextureId) {
    m_imageShape = imageShape;
    m_clObjects = clObjects;
    m_format = format;
    m_allowAccumulation = allowAccumulation;
    m_clGlInterop = glTextureId != 0;

    if (m_clGlInterop) {
        createImageBuffers(glTextureId);
    } else {
        createImageBuffers();
    }

    if (m_format == Format::RGBA8 && m_allowAccumulation) {
        printf("WARN: Using RGBA8 format with accumulation gives bad results\n");
    }
}


void Raytracer::renderScene(const internal::Scene& scene, const internal::Camera& camera, const Config& config) {
    if (m_kernels.count(config) == 0) {
        createClKernels(config);
    }

    cl::Kernel raytracerKernel = m_kernels[config];

    raytracerKernel.setArg(0, sizeof(internal::Camera), &camera);
    raytracerKernel.setArg(1, sizeof(internal::SceneExtra), &scene.extra);
    raytracerKernel.setArg(2, scene.objectsBuffer);
    raytracerKernel.setArg(3, scene.materialsBuffer);
    raytracerKernel.setArg(4, sizeof(uint32_t), &m_frameCount);
    
    if (m_clGlInterop && !m_allowAccumulation) {
        raytracerKernel.setArg(5, m_frameImageGl);
    } else {
        raytracerKernel.setArg(5, m_frameImage);
    }

    m_clObjects.queue.enqueueNDRangeKernel(
        raytracerKernel,
        cl::NullRange,
        cl::NDRange(m_imageShape.x * m_imageShape.y),
        cl::NullRange
    );
    m_clObjects.queue.finish();
}


void Raytracer::readPixels(void* outBuffer) const {
    cl::size_t<3> region;
    region[0] = m_imageShape.x;
    region[1] = m_imageShape.y;
    region[2] = 1;

    if (m_allowAccumulation) {
        m_clObjects.queue.enqueueReadImage(m_accumImage, true, cl::size_t<3>(), region, 0, 0, outBuffer);
    } else {
        m_clObjects.queue.enqueueReadImage(m_frameImage, true, cl::size_t<3>(), region, 0, 0, outBuffer);
    }
}


bool Raytracer::saveAsImage(const char* filepath) const {
    if (m_format != Format::RGBA8) {
        printf("ERROR (`Raytracer::saveAsImage`): Cannot save image for Format::%d\n", m_format);
        return false;
    }

    uint8_t* imageBuffer = new uint8_t[getPixelBufferSize()];
    readPixels(imageBuffer);
    bool res = stbi_write_png(filepath, m_imageShape.x, m_imageShape.y, 4, imageBuffer, 0);
    delete[] imageBuffer;
    return res;
}


void Raytracer::accumulatePixels() {
    if (!m_allowAccumulation) {
        return;
    }

    m_accumulatorKernel.setArg(0, m_frameImage);
    m_accumulatorKernel.setArg(3, sizeof(uint32_t), &m_frameCount);
    m_accumulatorKernel.setArg(4, sizeof(uint32_t), &m_imageShape.x);

    if (m_clGlInterop) {
        m_accumulatorKernel.setArg(1, m_accumImageGl);
        m_accumulatorKernel.setArg(2, m_accumImageGl);
    } else {
        m_accumulatorKernel.setArg(1, m_accumImage);
        m_accumulatorKernel.setArg(2, m_accumImage);
    }

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
        case Format::RGBA16F:
            return numPixels * 4 * sizeof(float) / 2;
        default:
            printf("ERROR (`Raytracer::getPixelBufferSize`): Not implement for Format::%d\n", m_format);
            return 0;
    }
}


void Raytracer::createImageBuffers() {
    int err[2] = {0, 0};
    float bufferSizeMB = (float) getPixelBufferSize() / (1024 * 1024);

    cl::ImageFormat imgFormat = getClImageFormat(m_format);
    m_frameImage = cl::Image2D(m_clObjects.context, CL_MEM_READ_WRITE, imgFormat, m_imageShape.x, m_imageShape.y, 0, nullptr, &err[0]);
    if (m_allowAccumulation) {
        m_accumImage = cl::Image2D(m_clObjects.context, CL_MEM_READ_WRITE, imgFormat, m_imageShape.x, m_imageShape.y, 0, nullptr, &err[1]);
    }

    if (err[0]) {
        printf("ERROR (`createImageBuffers`): Unable to allocate %.3f MB for m_frameImage\n", bufferSizeMB);
    } else {
        printf("INFO (`createImageBuffers`): Allocated %.3f MB for m_frameImage\n", bufferSizeMB);
    }

    if (m_allowAccumulation) {
        if (err[1]) {
            printf("ERROR (`createImageBuffers`): Unable to allocate %.3f MB for m_accumImage\n", bufferSizeMB);
        } else {
            printf("INFO (`createImageBuffers`): Allocated %.3f MB for m_accumImage\n", bufferSizeMB);
        }
    }
}


void Raytracer::createImageBuffers(uint32_t glTextureId) {
    int err[2] = {0, 0};
    float bufferSizeMB = (float) getPixelBufferSize() / (1024 * 1024);

    cl::ImageFormat imgFormat = getClImageFormat(m_format);

    if (m_allowAccumulation) {
        m_frameImage = cl::Image2D(m_clObjects.context, CL_MEM_READ_WRITE, imgFormat, m_imageShape.x, m_imageShape.y, 0, nullptr, &err[0]);
        // GL_TEXTURE_2D = 0x0DE1
        m_accumImageGl = cl::ImageGL(m_clObjects.context, CL_MEM_READ_WRITE, 0x0DE1, 0, glTextureId, &err[1]);
    } else {
        m_frameImageGl = cl::ImageGL(m_clObjects.context, CL_MEM_READ_WRITE, 0x0DE1, 0, glTextureId, &err[0]);
    }

    if (err[0]) {
        if (m_allowAccumulation) {
            printf("ERROR (`createImageBuffers`): Unable to allocate %.3f MB for m_frameImage\n", bufferSizeMB);
        } else {
            printf("ERROR (`createImageBuffers`): Unable to create m_frameImageGl with textureId = %d\n", glTextureId);
        }
    } else {
        if (m_allowAccumulation) {
            printf("INFO (`createImageBuffers`): Allocated %.3f MB for m_frameImage\n", bufferSizeMB);
        } else {
            printf("INFO (`createImageBuffers`): Created m_frameImageGl with textureId = %d\n", glTextureId);
        }
    }

    if (m_allowAccumulation) {
        if (err[1]) {
            printf("ERROR (`createImageBuffers`): Unable to create m_accumImageGl with textureId = %d\n", glTextureId);
        } else {
            printf("INFO (`createImageBuffers`): Created m_accumImageGl with textureId = %d\n", glTextureId);
        }
    }
}


void Raytracer::createClKernels(const rt::Config& config) {
    std::string raytracerFileSource = readFile("kernels/raytracer.cl");
    std::string accumulatorFileSource = readFile("kernels/accumulator.cl");

    if (raytracerFileSource.empty() || accumulatorFileSource.empty()) {
        printf("ERROR (`createClKernels`): Something went wrong while reading the Cl files\n");
        return;
    }

    cl::Program raytracerProgram = cl::Program(raytracerFileSource);
    cl::Program accumulatorProgram = cl::Program(accumulatorFileSource);

    std::string buildFlags = makeClProgramsBuildFlags(config);
    printf("INFO (`createClKernels`): (Re)building Cl Programs with flags: %s\n", buildFlags.c_str());

    if (raytracerProgram.build(buildFlags.c_str()) || accumulatorProgram.build(buildFlags.c_str())) {
        printf("ERROR (`createClKernels`): Encountered error while building Cl programs\n");
        std::string raytracerBuildLog = raytracerProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_clObjects.device);
        std::string accumulatorBuildLog = accumulatorProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_clObjects.device);
        printf("Build log for raytracer:\n%s\n", raytracerBuildLog.c_str());
        printf("--------------------------\n");
        printf("Build log for accumulator:\n%s\n", accumulatorBuildLog.c_str());
    } else {
        printf("INFO (`createClKernels`): Built Cl programs successfully\n");
        m_kernels[config] = cl::Kernel(raytracerProgram, "raytraceScene");
        m_accumulatorKernel = cl::Kernel(accumulatorProgram, "accumulateFrameData");
    }
}


std::string Raytracer::makeClProgramsBuildFlags(const rt::Config& config) const {
    std::stringstream stream;

    stream << " -DCONFIG__SAMPLE_COUNT=" << config.sampleCount;
    stream << " -DCONFIG__BOUNCE_LIMIT=" << config.bounceLimit;

    return stream.str();
}

}
