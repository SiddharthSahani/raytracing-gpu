
#pragma once

#include "src/raytracer/internal/scene.h"
#include "src/raytracer/objects.h"
#include "src/raytracer/material.h"
#include <vector>
#include <algorithm> // std::find


namespace rt {

struct Scene {
    std::vector<Object> objects;
    glm::vec3 backgroundColor;
};


static internal::Scene convert(const Scene& scene, cl::Context clContext, cl::CommandQueue clQueue) {
    // 1. Grouping common materials
    std::vector<std::shared_ptr<internal::Material>> uniqueMaterials;
    std::vector<uint32_t> materialIndices(scene.objects.size());
    {
        auto findMaterial = [&](std::shared_ptr<internal::Material> mat) {
            return std::find(uniqueMaterials.begin(), uniqueMaterials.end(), mat);
        };

        for (int objIdx = 0; objIdx < scene.objects.size(); objIdx++) {
            std::shared_ptr<internal::Material> mat = scene.objects[objIdx].material;
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
    uint32_t objectsBufferSize = scene.objects.size() * sizeof(internal::Object);
    uint32_t materialsBufferSize = uniqueMaterials.size() * sizeof(internal::Material);
    uint32_t sceneBufferSize = objectsBufferSize + materialsBufferSize;
    cl::Buffer objectsBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY, objectsBufferSize, nullptr, &err);
    cl::Buffer materialsBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY, materialsBufferSize, nullptr, &err);

    if (err) {
        printf("ERROR: Unable to allocate buffers for [size: %.3f KB]\n", (float) sceneBufferSize / 1024);
        internal::Scene scene;
        scene.extra.numObjects = 0;
        scene.extra.backgroundColor = {1.0f, 0.0f, 0.0f, 1.0f};
        return scene;
    }

    printf("INFO: Allocated buffers for [size %.3f KB]\n", (float) sceneBufferSize / 1024);

    std::vector<internal::Object> internalObjects(scene.objects.size());
    std::vector<internal::Material> materials(uniqueMaterials.size());
    for (int i = 0; i < scene.objects.size(); i++) {
        internalObjects[i] = convert(scene.objects[i]);
        internalObjects[i].materialIndex = materialIndices[i];
    }
    for (int i = 0; i < materials.size(); i++) {
        materials[i] = *uniqueMaterials[i];
    }
    clQueue.enqueueWriteBuffer(objectsBuffer, true, 0, objectsBufferSize, internalObjects.data());
    clQueue.enqueueWriteBuffer(materialsBuffer, true, 0, materialsBufferSize, materials.data());

    internal::Scene out;
    out.objectsBuffer = objectsBuffer;
    out.materialsBuffer = materialsBuffer;
    out.extra.numObjects = scene.objects.size();
    out.extra.backgroundColor = {scene.backgroundColor.r, scene.backgroundColor.g, scene.backgroundColor.b, 1.0f};
    return out;
}

}
