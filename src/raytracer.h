
#pragma once

#include "src/rtlog.h"
#include "src/cl_utils.h"
#include "src/raytracer/scene.h"
#include "src/raytracer/camera.h"
#include <sstream>


namespace rt {

// modes: once? , realtime? , video?
// formats: R32G32B32A32? , R8G8B8?

enum class PixelFormat {
    R8G8B8A8,     // (4x1= 4) ints
    R32G32B32A32, // (4*4=16) floats
};


enum class RaytracingMode {
    SINGLE,  // just a single frame
    MULTIPLE // multiple frames
};


struct Config {
    cl_uint sampleCount;
    cl_uint bounceLimit;

    bool operator!=(const Config& other) const {
        return (sampleCount != other.sampleCount) | (bounceLimit != other.bounceLimit);
    }
};


struct CL_Objects {
    cl::Device device;
    cl::Context context;
    cl::CommandQueue queue;
    cl::Kernel renderKernel;
    cl::Kernel accumulateKernel;
};


namespace internal {

struct SceneParams {
    cl_float3 backgroundColor;
    cl_uint objectsCount;
};

};


struct CompiledScene {
    cl::Buffer objectsBuffer;
    cl::Buffer materialsBuffer;
    cl_float3 backgroundColor;
    cl_uint objectsCount;
};


Config DEFAULT_CONFIG = {
    .sampleCount = 128,
    .bounceLimit = 5
};


class Raytracer {

    public:
        Raytracer(uint32_t viewportWidth, uint32_t viewportHeight, PixelFormat format = PixelFormat::R32G32B32A32, RaytracingMode mode = RaytracingMode::SINGLE);
        CompiledScene compileScene(const Scene& scene) const;
        void renderScene(const CompiledScene& scene, const Camera& camera, const Config& config);
        void readPixels(void* out) const;
        void accumulatePixels();
        uint32_t getPixelBufferSize() const;
        void resetFrameCount() { m_frameCount = 1; }
        PixelFormat getPixelFormat() const { return m_pixelFormat; }
        uint32_t getViewportWidth() const { return m_viewportWidth; }
        uint32_t getViewportHeight() const { return m_viewportHeight; }
        uint32_t getFrameCount() const { return m_frameCount; }
        bool isValid() const { return m_isValid; }

    private:
        void initializeClMembers();
        void makeClKernel();
        void createPixelBuffers();
        std::string makeProgramBuildFlags() const;

    private:
        bool m_isValid = true;
        uint32_t m_viewportWidth, m_viewportHeight;
        PixelFormat m_pixelFormat;
        RaytracingMode m_mode;
        CL_Objects m_cl;
        cl::Buffer m_pixelBuffer;
        cl::Buffer m_accumPixelBuffer;
        Config m_lastConfig; // to not rebuild the cl program again for same config
        uint32_t m_frameCount = 1;

};


Raytracer::Raytracer(uint32_t viewportWidth, uint32_t viewportHeight, PixelFormat format, RaytracingMode mode) {
    m_viewportWidth = viewportWidth;
    m_viewportHeight = viewportHeight;
    m_pixelFormat = format;
    m_mode = mode;
    m_lastConfig = DEFAULT_CONFIG;
    initializeClMembers();
    makeClKernel();
    createPixelBuffers();
}


void Raytracer::initializeClMembers() {
    if (!chooseClDevice(m_cl.device)) {
        RT_LOG("Unable to choose Cl Device");
        m_isValid = false;
        return;
    }

    m_cl.context = cl::Context(m_cl.device);
    m_cl.queue = cl::CommandQueue(m_cl.context, m_cl.device);
}


void Raytracer::makeClKernel() {
    std::string mainSource = readFile("kernels/renderer.cl");
    std::string accumulateSource = readFile("kernels/accumulate.cl");
    cl::Program mainProgram(mainSource);
    cl::Program accumulateProgram(accumulateSource);

    std::string buildFlags = makeProgramBuildFlags();

    RT_LOG("(Re)building Cl Program with flags: %s\n", buildFlags.c_str());

    if (mainProgram.build(buildFlags.c_str()) || (m_mode == RaytracingMode::MULTIPLE && accumulateProgram.build(buildFlags.c_str()))) {
        RT_LOG("Error while building Cl program");
        RT_LOG("Build Log for main:\n%s\n", mainProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_cl.device).c_str());
        if (m_mode == RaytracingMode::MULTIPLE) {
            RT_LOG("Build Log for accumulate:\n%s\n", accumulateProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(m_cl.device).c_str());
        }
        m_isValid = false;
    } else {
        RT_LOG("Built Cl Program successfully");
        m_cl.renderKernel = cl::Kernel(mainProgram, "renderScene");
        m_cl.accumulateKernel = cl::Kernel(accumulateProgram, "accumulateFrameData");
    }
}


void Raytracer::createPixelBuffers() {
    int err;
    uint32_t bufferSize = getPixelBufferSize();

    m_pixelBuffer = cl::Buffer(m_cl.context, CL_MEM_READ_WRITE, bufferSize, nullptr, &err);
    if (m_mode == RaytracingMode::MULTIPLE) {
        m_accumPixelBuffer = cl::Buffer(m_cl.context, CL_MEM_READ_WRITE, bufferSize, nullptr, &err);
    }

    uint32_t totalBufferSize = bufferSize * (m_mode == RaytracingMode::SINGLE ? 1 : 2);

    if (err) {
        RT_LOG("Unable to allocate buffers of size %.3f MB\n", (float) totalBufferSize / (1024*1024));
        m_isValid = false;
    } else {
        RT_LOG("Allocated buffers of size %.3f MB\n", (float) totalBufferSize / (1024*1024));
    }
}


void Raytracer::renderScene(const CompiledScene& scene, const Camera& camera, const Config& config) {
    if (!m_isValid) {        
        RT_LOG("Can not render scene as this instance is not valid");
        return;
    }
    if (config != m_lastConfig) {
        m_lastConfig = config;
        makeClKernel();
    }

    // m_cl.renderKernel.setArg(0, sizeof(Camera), &camera);
    // m_cl.renderKernel.setArg(1, sizeof(CompiledScene), &scene);
    // m_cl.renderKernel.setArg(2, m_pixelBuffer);
    // m_cl.renderKernel.setArg(3, sizeof(uint32_t), &m_frameCount);
    internal::SceneParams params = {
        .backgroundColor = scene.backgroundColor,
        .objectsCount = scene.objectsCount
    };

    m_cl.renderKernel.setArg(0, sizeof(Camera), &camera);
    m_cl.renderKernel.setArg(1, sizeof(internal::SceneParams), &params);
    m_cl.renderKernel.setArg(2, scene.objectsBuffer);
    m_cl.renderKernel.setArg(3, scene.materialsBuffer);
    m_cl.renderKernel.setArg(4, sizeof(uint32_t), &m_frameCount);
    m_cl.renderKernel.setArg(5, m_pixelBuffer);

    m_cl.queue.enqueueNDRangeKernel(
        m_cl.renderKernel,
        cl::NullRange,
        cl::NDRange(m_viewportWidth * m_viewportHeight),
        cl::NullRange
    );

    m_cl.queue.finish();
}


void Raytracer::readPixels(void* out) const {
    uint32_t bufferSize = getPixelBufferSize();

    if (m_mode == RaytracingMode::SINGLE) {
        m_cl.queue.enqueueReadBuffer(m_pixelBuffer, true, 0, bufferSize, out);
    } else {
        m_cl.queue.enqueueReadBuffer(m_accumPixelBuffer, true, 0, bufferSize, out);
    }
}


void Raytracer::accumulatePixels() {
    if (m_mode == RaytracingMode::SINGLE) {
        RT_LOG("Can not use this function in single mode");
        return;
    }

    m_cl.accumulateKernel.setArg(0, m_accumPixelBuffer);
    m_cl.accumulateKernel.setArg(1, m_pixelBuffer);
    m_cl.accumulateKernel.setArg(2, sizeof(m_frameCount), &m_frameCount);

    m_cl.queue.enqueueNDRangeKernel(
        m_cl.accumulateKernel,
        cl::NullRange,
        cl::NDRange(m_viewportWidth * m_viewportHeight),
        cl::NullRange
    );

    m_cl.queue.finish();

    m_frameCount++;
}


uint32_t Raytracer::getPixelBufferSize() const {
    uint32_t pixelSize;

    switch (m_pixelFormat) {
        case PixelFormat::R8G8B8A8:
            pixelSize = 4;
            break;
        case PixelFormat::R32G32B32A32:
            pixelSize = 16;
            break;
        default:
            RT_LOG("Invalid pixel format encountered in `Raytracer::getPixelBufferSize()`");
            break;
    }

    return m_viewportWidth * m_viewportHeight * pixelSize;
}


std::string Raytracer::makeProgramBuildFlags() const {
    std::stringstream stream;

    stream << "-DCONFIG__SAMPLE_COUNT=" << m_lastConfig.sampleCount << " ";
    stream << "-DCONFIG__BOUNCE_LIMIT=" << m_lastConfig.bounceLimit << " ";
    
    switch (m_pixelFormat) {
        case PixelFormat::R8G8B8A8:
            stream << "-DPIXEL_FORMAT__R8G8B8A8 ";
            break;
        case PixelFormat::R32G32B32A32:
            stream << "-DPIXEL_FORMAT__R32G32B32A32 ";
            break;
        default:
            RT_LOG("Invalid pixel format: %d defaulting to R32G32B32A32\n", (int) m_pixelFormat);
            stream << "-DPIXEL_FORMAT__R32G32B32A32 ";
            break;
    }

    return stream.str();
}


CompiledScene Raytracer::compileScene(const Scene& scene) const {
    
    // 1. Grouping up common materials
    std::vector<const Material*> uniqueMaterials;
    std::vector<uint32_t> materialIndices(scene.objects.size());
    {
        auto findMaterial = [&](const Material* mat) {
            return std::find(uniqueMaterials.begin(), uniqueMaterials.end(), mat);
        };

        for (int objIdx = 0; objIdx < scene.objects.size(); objIdx++) {
            const Material* mat = scene.objects[objIdx].material.get();
            auto matLocation = findMaterial(mat);

            if (matLocation == uniqueMaterials.end()) {
                uniqueMaterials.push_back(mat);
                materialIndices[objIdx] = uniqueMaterials.size() - 1;
            } else {
                materialIndices[objIdx] = matLocation - uniqueMaterials.begin();
            }
        }
    }

    // 2. Creating buffers
    int err;

    uint32_t objectsBufferSize = scene.objects.size() * sizeof(internal::Object);
    uint32_t materialsBufferSize = uniqueMaterials.size() * sizeof(Material);
    uint32_t sceneBufferSize = objectsBufferSize + materialsBufferSize;
    cl::Buffer objectsBuffer = cl::Buffer(m_cl.context, CL_MEM_READ_ONLY, objectsBufferSize, nullptr, &err);
    cl::Buffer materialsBuffer = cl::Buffer(m_cl.context, CL_MEM_READ_ONLY, materialsBufferSize, nullptr, &err);

    if (err) {
        RT_LOG("Unable to allocate buffers for scene [size: %.3f KB]\n", (float) sceneBufferSize / 1024);
    } else {
        // RT_LOG("Object Buffer Size: %.3f B [size: %d]\n", (float) objectsBufferSize, scene.objects.size());
        // RT_LOG("Material Buffer Size: %.3f B [size: %d]\n", (float) materialsBufferSize, uniqueMaterials.size());
        RT_LOG("Allocated buffers for scene [size %.3f KB]\n", (float) sceneBufferSize / 1024);

        std::vector<internal::Object> objects(scene.objects.size());
        std::vector<Material> materials(uniqueMaterials.size());
        for (int i = 0; i < objects.size(); i++) {
            objects[i] = internal::convert(scene.objects[i]);
            objects[i].materialIndex = materialIndices[i];
        }
        for (int i = 0; i < materials.size(); i++) {
            materials[i] = *uniqueMaterials[i];
        }
        m_cl.queue.enqueueWriteBuffer(objectsBuffer, true, 0, objectsBufferSize, objects.data());
        m_cl.queue.enqueueWriteBuffer(materialsBuffer, true, 0, materialsBufferSize, materials.data());
    }

    if (err) {
        CompiledScene cScene;
        cScene.objectsCount = 0;
        return cScene;
    }

    CompiledScene cScene;
    cScene.objectsBuffer = objectsBuffer;
    cScene.materialsBuffer = materialsBuffer;
    cScene.objectsCount = scene.objects.size();
    cScene.backgroundColor = {scene.backgroundColor.r, scene.backgroundColor.g, scene.backgroundColor.b, 1.0f};
    return cScene;
}

}
