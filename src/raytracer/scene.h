
#pragma once

#include "src/rtlog.h"
#include "src/raytracer/internal/scene.h"
#include "src/raytracer/objects.h"
#include "src/raytracer/material.h"
#include <vector>


namespace rt {

struct Scene {
    std::vector<Object> objects;
    glm::vec3 backgroundColor;

    internal::Scene convert(cl::Context clContext, cl::CommandQueue clQueue);
};


internal::Scene Scene::convert(cl::Context clContext, cl::CommandQueue clQueue) {
    // 1. Grouping common materials
    std::vector<std::shared_ptr<internal::Material>> uniqueMaterials;
    std::vector<uint32_t> materialIndices(objects.size());
    {
        auto findMaterial = [&](std::shared_ptr<internal::Material> mat) {
            return std::find(uniqueMaterials.begin(), uniqueMaterials.end(), mat);
        };

        for (int objIdx = 0; objIdx < objects.size(); objIdx++) {
            std::shared_ptr<internal::Material> mat = objects[objIdx].material;
            auto matLocation = findMaterial(mat);

            if (matLocation == uniqueMaterials.end()) {
                uniqueMaterials.push_back(mat);
                materialIndices[objIdx] = uniqueMaterials.size() - 1;
            } else {
                materialIndices[objIdx] = matLocation - uniqueMaterials.begin();
            }
        }
    }

    // 2. Creating scene buffers
    int err;
    uint32_t objectsBufferSize = objects.size() * sizeof(internal::Object);
    uint32_t materialsBufferSize = uniqueMaterials.size() * sizeof(internal::Material);
    uint32_t sceneBufferSize = objectsBufferSize + materialsBufferSize;
    cl::Buffer objectsBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY, objectsBufferSize, nullptr, &err);
    cl::Buffer materialsBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY, materialsBufferSize, nullptr, &err);

    if (err) {
        RT_LOG("Unable to allocate buffers for [size: %.3f KB]\n", (float) sceneBufferSize / 1024);
        internal::Scene scene;
        scene.extra.numObjects = 0;
        scene.extra.backgroundColor = {1.0f, 0.0f, 0.0f, 1.0f};
        return scene;
    }

    RT_LOG("Allocated buffers for [size %.3f KB]\n", (float) sceneBufferSize / 1024);

    std::vector<internal::Object> internalObjects(objects.size());
    std::vector<internal::Material> materials(uniqueMaterials.size());
    for (int i = 0; i < objects.size(); i++) {
        internalObjects[i] = objects[i].convert();
        internalObjects[i].materialIndex = materialIndices[i];
    }
    for (int i = 0; i < materials.size(); i++) {
        materials[i] = *uniqueMaterials[i];
    }
    clQueue.enqueueWriteBuffer(objectsBuffer, true, 0, objectsBufferSize, internalObjects.data());
    clQueue.enqueueWriteBuffer(materialsBuffer, true, 0, materialsBufferSize, materials.data());

    internal::Scene scene;
    scene.objectsBuffer = objectsBuffer;
    scene.materialsBuffer = materialsBuffer;
    scene.extra.numObjects = objects.size();
    scene.extra.backgroundColor = {backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f};
    return scene;
}

}
