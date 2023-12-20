
#pragma once

#include <CL/opencl.h>
#include <glm/vec3.hpp>
#include <memory>
#include <stdio.h>


namespace rt {

struct Material {
    cl_float3 color;
    cl_float smoothness;
    cl_float3 emissionColor;
};


std::shared_ptr<Material> createMaterial(const glm::vec3& color, float smoothness) {
    Material material = {
        .color = {color.r, color.g, color.b, 1.0f},
        .smoothness = smoothness,
        .emissionColor = {0.0f, 0.0f, 0.0f, 0.0f}
    };
    return std::make_shared<Material>(material);
}


std::shared_ptr<Material> createEmissiveMaterial(const glm::vec3& color, float power) {
    glm::vec3 emissionColor = color * power;
    Material material = {
        .color = {color.r, color.g, color.b, 1.0f},
        .smoothness = 0.0f,
        .emissionColor = {emissionColor.r, emissionColor.g, emissionColor.b, 1.0f}
    };
    return std::make_shared<Material>(material);
}


void printMaterialInfo(const Material& material) {
    printf("  color: %f %f %f\n", material.color.x, material.color.y, material.color.z);
    printf("  smoothness: %f\n", material.smoothness);
    printf("  emisssionColor: %f %f %f\n", material.emissionColor.x, material.emissionColor.y, material.emissionColor.z);
}

}
