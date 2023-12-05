
#pragma once

#include <CL/opencl.h>
#include <glm/vec3.hpp>
#include <memory>
#include <variant>
#include <stdio.h>


namespace rt {

struct Sphere {
    cl_float3 position;
    cl_float radius;
};


struct Triangle {
    cl_float3 v0;
    cl_float3 v1;
    cl_float3 v2;
};


struct Material;

struct Object {
    std::variant<Sphere, Triangle> internal;
    std::shared_ptr<Material> material;
};


Object createSphere(const glm::vec3& position, float radius, std::shared_ptr<Material> material) {
    Sphere sphere = {
        .position = {position.x, position.y, position.z, 1.0f},
        .radius = radius
    };
    Object object = {
        .internal = sphere,
        .material = material
    };
    return object;
}


Object createTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, std::shared_ptr<Material> material) {
    Triangle triangle = {
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


void printSphereInfo(const Sphere& sphere) {
    printf("  type: Sphere\n");
    printf("    position: %f %f %f\n", sphere.position.x, sphere.position.y, sphere.position.z);
    printf("    radius: %f\n", sphere.radius);
}


void printTriangleInfo(const Triangle& triangle) {
    printf("  type: Triangle\n");
    printf("    v0: %f %f %f\n", triangle.v0.x, triangle.v0.y, triangle.v0.z);
    printf("    v1: %f %f %f\n", triangle.v1.x, triangle.v1.y, triangle.v1.z);
    printf("    v2: %f %f %f\n", triangle.v2.x, triangle.v2.y, triangle.v2.z);
}

}
