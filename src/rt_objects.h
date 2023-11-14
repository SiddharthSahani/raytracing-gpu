
#pragma once

#include <glm/glm.hpp>
#include <CL/opencl.hpp>

#define MAX_SPHERES 5
#define MAX_MATERIALS 5


namespace rt {

struct RendererConfig {
    cl_uint samples;
    cl_uint bounces;
};


struct Sphere {
    glm::vec3 position;
    float radius;
    uint32_t material_idx;
};


struct Material {
    glm::vec3 color;
};


struct Scene {
    uint32_t num_spheres;
    glm::vec3 sky_color;
    Sphere spheres[MAX_SPHERES];
    Material materials[MAX_MATERIALS];
};


struct clSphere {
    cl_float3 position;
    cl_float radius;
    cl_uint material_idx;
};


struct clMaterial {
    cl_float3 color;
};


struct clScene {
    cl_uint num_spheres;
    cl_float3 sky_color;
    clSphere spheres[MAX_SPHERES];
    clMaterial materials[MAX_MATERIALS];
};


clSphere to_clSphere(const Sphere& _sphere) {
    clSphere sphere;
    sphere.position = {_sphere.position[0], _sphere.position[1], _sphere.position[2], 1.0f};
    sphere.radius = _sphere.radius;
    sphere.material_idx = _sphere.material_idx;
    return sphere;
} 


clMaterial to_clMaterial(const Material& _material) {
    clMaterial material;
    material.color = {_material.color[0], _material.color[1], _material.color[2], 1.0f};
    return material;
}


clScene to_clScene(const Scene& _scene) {
    clScene scene;
    scene.num_spheres = _scene.num_spheres;
    assert(scene.num_spheres <= MAX_SPHERES);
    for (int i = 0; i < MAX_SPHERES; i++) {
        scene.spheres[i] = to_clSphere(_scene.spheres[i]);
    }
    for (int i = 0; i < MAX_MATERIALS; i++) {
        scene.materials[i] = to_clMaterial(_scene.materials[i]);
    }
    scene.sky_color = {_scene.sky_color[0], _scene.sky_color[1], _scene.sky_color[2], 1.0f};
    return scene;
}

}
