
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

    if (m_format == Format::RGBA8 && m_allowAccumulation) {
        printf("WARN: Using RGBA8 format with accumulation gives bad results\n");
    }

    createImageBuffers(glTextureId);
    createClPrograms();
}


void Raytracer::renderScene(const internal::Scene& scene, const internal::Camera& camera, const Config& config) {
    m_raytracerKernel.setArg(0, sizeof(internal::Camera), &camera);
    m_raytracerKernel.setArg(1, sizeof(internal::SceneExtra), &scene.extra);
    m_raytracerKernel.setArg(2, scene.objectsBuffer);
    m_raytracerKernel.setArg(3, scene.materialsBuffer);
    m_raytracerKernel.setArg(4, sizeof(uint32_t), &m_frameCount);
    m_raytracerKernel.setArg(5, sizeof(uint32_t), &config.sampleCount);
    m_raytracerKernel.setArg(6, sizeof(uint32_t), &config.bounceLimit);

    if (m_clGlInterop && !m_allowAccumulation) {
        m_raytracerKernel.setArg(7, m_frameImageGl);
    } else {
        m_raytracerKernel.setArg(7, m_frameImage);
    }

    m_clObjects.queue.enqueueNDRangeKernel(
        m_raytracerKernel,
        cl::NullRange,
        cl::NDRange(m_imageShape.x * m_imageShape.y),
        cl::NullRange
    );
    m_clObjects.queue.finish();
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

    int x = m_clObjects.queue.enqueueNDRangeKernel(
        m_accumulatorKernel,
        cl::NullRange,
        cl::NDRange(m_imageShape.x * m_imageShape.y),
        cl::NullRange
    );
    // printf("cl err: %d\n", x);
    // printf("%d %d %d\n", m_frameImage(), m_accumImage(), m_accumImageGl());
    m_clObjects.queue.finish();

    m_frameCount++;
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

    uint8_t* imageBuffer = new uint8_t[getImageSize()];
    readPixels(imageBuffer);
    bool res = stbi_write_png(filepath, m_imageShape.x, m_imageShape.y, 4, imageBuffer, 0);
    delete[] imageBuffer;
    return res;
}


uint32_t Raytracer::getImageSize() const {
    uint32_t numPixels = m_imageShape.x * m_imageShape.y;
    switch (m_format) {
        case Format::RGBA8:
            return numPixels * 4 * sizeof(uint8_t);
        case Format::RGBA32F:
            return numPixels * 4 * sizeof(float);
        case Format::RGBA16F:
            return numPixels * 4 * sizeof(float) / 2;
        default:
            printf("ERROR (`Raytracer::getImageSize`): Not implement for Format::%d\n", m_format);
            return 0;
    }
}


void Raytracer::createFrameImage(uint32_t glTextureId) {
    cl::ImageFormat format = getClImageFormat(m_format);
    float imageSizeMB = (float) getImageSize() / (1024 * 1024);
    int err;

    if (glTextureId == 0) {
        m_frameImage = cl::Image2D(m_clObjects.context, CL_MEM_READ_WRITE, format, m_imageShape.x, m_imageShape.y, 0, nullptr, &err);
        if (err) {
            printf("ERROR (`Raytracer::createFrameImage`): Unable to allocate %.3f MB for m_frameImage\n", imageSizeMB);
        } else {
            printf("INFO (`Raytracer::createFrameImage`): Allocated %.3f MB for m_frameImage\n", imageSizeMB);
        }
    } else {
        // GL_TEXTURE_2D = 0x0DE1
        m_frameImageGl = cl::ImageGL(m_clObjects.context, CL_MEM_READ_WRITE, 0x0DE1, 0, glTextureId, &err);
        if (err) {
            printf("ERROR (`Raytracer::createFrameImage`): Unable to create m_frameImageGl with glTextureId = %d\n", glTextureId);
        } else {
            printf("INFO (`Raytracer::createFrameImage`): Created m_frameImageGl with glTextureId = %d\n", glTextureId);
        }
    }
}


void Raytracer::createAccumImage(uint32_t glTextureId) {
    cl::ImageFormat format = getClImageFormat(m_format);
    float imageSizeMB = (float) getImageSize() / (1024 * 1024);
    int err;

    if (glTextureId == 0) {
        m_accumImage = cl::Image2D(m_clObjects.context, CL_MEM_READ_WRITE, format, m_imageShape.x, m_imageShape.y, 0, nullptr, &err);
        if (err) {
            printf("ERROR (`Raytracer::createAccumImage`): Unable to allocate %.3f MB for m_accumImage\n", imageSizeMB);
        } else {
            printf("INFO (`Raytracer::createAccumImage`): Allocated %.3f MB for m_accumImage\n", imageSizeMB);
        }
    } else {
        // GL_TEXTURE_2D = 0x0DE1
        m_accumImageGl = cl::ImageGL(m_clObjects.context, CL_MEM_READ_WRITE, 0x0DE1, 0, glTextureId, &err);
        if (err) {
            printf("ERROR (`Raytracer::createAccumImage`): Unable to create m_accumImageGl with glTextureId = %d\n", glTextureId);
        } else {
            printf("INFO (`Raytracer::createAccumImage`): Created m_accumImageGl with glTextureId = %d\n", glTextureId);
        }
    }
}


void Raytracer::createImageBuffers(uint32_t glTextureId) {
    if (glTextureId == 0) {
        createFrameImage(0);
        if (m_allowAccumulation) {
            createAccumImage(0);
        }
    } else {
        if (m_allowAccumulation) {
            createFrameImage(0);
            createAccumImage(glTextureId);
        } else {
            createFrameImage(glTextureId);
        }
    }
}


void Raytracer::createClPrograms() {
    std::string raytracerFileSource = readFile("kernels/raytracer.cl");
    std::string accumulatorFileSource = readFile("kernels/accumulator.cl");

    if (raytracerFileSource.empty() || accumulatorFileSource.empty()) {
        printf("ERROR (`createClPrograms`): Something went wrong while reading the Cl files\n");
        return;
    }

    cl::Program raytracerProgram = cl::Program(raytracerFileSource);
    cl::Program accumulatorProgram = cl::Program(accumulatorFileSource);

    std::string buildFlags = makeClProgramsBuildFlags();
    printf("INFO (`createClPrograms`): Building Cl Programs with flags: %s\n", buildFlags.c_str());

    if (raytracerProgram.build(buildFlags.c_str()) || accumulatorProgram.build(buildFlags.c_str())) {
        printf("ERROR (`createClPrograms`): Encountered error while building Cl programs\n");
        std::string raytracerBuildLog = raytracerProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_clObjects.device);
        std::string accumulatorBuildLog = accumulatorProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_clObjects.device);
        printf("Build log for raytracer:\n%s\n", raytracerBuildLog.c_str());
        printf("--------------------------\n");
        printf("Build log for accumulator:\n%s\n", accumulatorBuildLog.c_str());
    } else {
        printf("INFO (`createClPrograms`): Built Cl programs successfully\n");
        m_raytracerKernel = cl::Kernel(raytracerProgram, "raytraceScene");
        m_accumulatorKernel = cl::Kernel(accumulatorProgram, "accumulateFrameData");
    }
}


std::string Raytracer::makeClProgramsBuildFlags() const {
    return "";
}

}
