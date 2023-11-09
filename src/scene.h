
#pragma once

#include <glm/glm.hpp>

#define MAX_SPHERES 5


namespace rt {

struct Sphere {
    glm::vec3 position;
    float radius;
    glm::vec3 color;
};


struct Scene {
    uint32_t num_spheres;
    Sphere spheres[MAX_SPHERES];
};


struct clSphere {
    cl_float3 position;
    cl_float radius;
    cl_float3 color;
};


struct clScene {
    cl_uint num_spheres;
    clSphere spheres[MAX_SPHERES];
};


clSphere to_clSphere(const Sphere& _sphere) {
    clSphere sphere;
    sphere.position = {_sphere.position[0], _sphere.position[1], _sphere.position[2], 1.0f};
    sphere.radius = _sphere.radius;
    sphere.color = {_sphere.color[0], _sphere.color[1], _sphere.color[2], 1.0f};
    return sphere;
} 


clScene to_clScene(const Scene& _scene) {
    clScene scene;
    scene.num_spheres = _scene.num_spheres;
    assert(scene.num_spheres <= MAX_SPHERES);
    for (int i = 0; i < scene.num_spheres; i++) {
        scene.spheres[i] = to_clSphere(_scene.spheres[i]);
    }
    return scene;
}

}
