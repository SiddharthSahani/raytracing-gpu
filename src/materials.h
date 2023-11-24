
#pragma once

#include <CL/opencl.hpp>
#include <glm/glm.hpp>
#include <memory>


namespace rt {

struct Material {
    cl_float3 color;
    cl_float smoothness;
};


std::shared_ptr<Material> createMaterial(const glm::vec3& color, float smoothness) {
    Material material = {
        .color = {color.r, color.g, color.b, 1.0f},
        .smoothness = smoothness
    };
    return std::make_shared<Material>(material);
}


void printMaterialInfo(const Material& material) {
    printf("  color: %f %f %f\n", material.color.x, material.color.y, material.color.z);
    printf("  smoothness: %f\n", material.smoothness);
}

}
