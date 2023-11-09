
#pragma once

#include <glm/glm.hpp>


namespace rt {

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};


struct clRay {
    cl_float3 origin;
    cl_float3 direction;
};


clRay to_clRay(const Ray& _ray) {
    clRay ray;
    ray.origin = {_ray.origin[0], _ray.origin[1], _ray.origin[2], 1.0f};
    ray.direction = {_ray.direction[0], _ray.direction[1], _ray.direction[2], 0.0f};
    return ray;
}

}
