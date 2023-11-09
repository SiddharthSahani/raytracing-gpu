
#pragma once

#include <glm/glm.hpp>

#define MAX_SPHERES 5


namespace rt {

struct Sphere {
    glm::vec4 position;
    float radius;
};


struct Scene {
    uint32_t num_spheres;
    Sphere spheres[MAX_SPHERES];
};


struct clSphere {
    cl_float3 position;
    cl_float radius;
};


struct clScene {
    cl_uint num_spheres;
    clSphere spheres[MAX_SPHERES];
};


clSphere to_clSphere(const Sphere& _sphere) {
    clSphere sphere;
    sphere.position = {_sphere.position[0], _sphere.position[1], _sphere.position[2], 1.0f};
    sphere.radius = _sphere.radius;
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
