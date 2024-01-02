
#pragma once

#include "src/raytracer/internal/material.h"
#include <glm/vec3.hpp>
#include <memory>


namespace rt {

std::shared_ptr<internal::Material> createMaterial(const glm::vec3& color, float smoothness) {
    internal::Material material = {
        .color = {color.r, color.g, color.b, 1.0f},
        .emissionColor = {0.0f, 0.0f, 0.0f, 0.0f},
        .smoothness = smoothness
    };
    return std::make_shared<internal::Material>(material);
}


std::shared_ptr<internal::Material> createEmissiveMaterial(const glm::vec3& color, float power) {
    glm::vec3 emissionColor = color * power;
    internal::Material material = {
        .color = {color.r, color.g, color.b, 1.0f},
        .emissionColor = {emissionColor.r, emissionColor.g, emissionColor.b, 1.0f},
        .smoothness = 0.0f
    };
    return std::make_shared<internal::Material>(material);
}

}
