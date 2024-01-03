
#pragma once

#include "src/raytracer/internal/objects.h"
#include "src/raytracer/internal/material.h"
#include <glm/vec3.hpp>
#include <memory>
#include <variant>


namespace rt {

struct Object {
    std::variant<internal::Sphere, internal::Triangle> internal;
    std::shared_ptr<internal::Material> material;

    internal::Object convert() const;
};


Object createSphere(const glm::vec3& position, float radius, std::shared_ptr<internal::Material> material) {
    internal::Sphere sphere = {
        .position = {position.x, position.y, position.z, 1.0f},
        .radius = radius
    };
    Object object = {
        .internal = sphere,
        .material = material
    };
    return object;
}


Object createTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, std::shared_ptr<internal::Material> material) {
    internal::Triangle triangle = {
        .v0 = {v0.x, v0.y, v0.z, 1.0f},
        .v1 = {v1.x, v1.y, v1.z, 1.0f},
        .v2 = {v2.x, v2.y, v2.z, 1.0f}
    };
    Object object = {
        .internal = triangle,
        .material = material
    };
    return object;
}


internal::Object Object::convert() const {
    internal::Object object;
    if (auto ptr = std::get_if<internal::Sphere>(&internal)) {
        object.sphere = *ptr;
        object.type = 0;
    } else if (auto ptr = std::get_if<internal::Triangle>(&internal)) {
        object.triangle = *ptr;
        object.type = 1;
    } else {
        printf("ERROR: While convert rt::Object to rt::internal::Object\n");
    }
    return object;
}

}
