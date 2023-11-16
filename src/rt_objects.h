
#pragma once

#include <glm/glm.hpp>
#include <CL/opencl.hpp>

#define MAX_OBJECTS 5
#define MAX_MATERIALS 5


namespace rt {

#define OBJECT_TYPE_SPHERE 0


struct Sphere {
    glm::vec3 position;
    float radius;
};


struct RendererConfig {
    cl_uint samples;
    cl_uint bounces;
};


struct Object {
    union {
        Sphere sphere;
    };
    uint32_t type;
    uint32_t material_idx;
};


struct Material {
    glm::vec3 color;
};


struct Scene {
    uint32_t num_objects;
    glm::vec3 sky_color;
    Object objects[MAX_OBJECTS];
    Material materials[MAX_MATERIALS];
};


struct clSphere {
    cl_float3 position;
    cl_float radius;
    cl_uint material_idx;
};


struct clObject {
    union {
        clSphere sphere;
    };
    cl_uint type;
    cl_uint material_idx;
};


struct clMaterial {
    cl_float3 color;
};


struct clScene {
    cl_uint num_objects;
    cl_float3 sky_color;
    clObject objects[MAX_OBJECTS];
    clMaterial materials[MAX_MATERIALS];
};


clSphere to_clSphere(const Sphere& _sphere) {
    clSphere sphere;
    sphere.position = {_sphere.position[0], _sphere.position[1], _sphere.position[2], 1.0f};
    sphere.radius = _sphere.radius;
    return sphere;
} 


clObject to_clObject(const Object& _object) {
    clObject object;
    object.type = _object.type;
    object.material_idx = _object.material_idx;
    switch (_object.type) {
        case OBJECT_TYPE_SPHERE:
            object.sphere = to_clSphere(_object.sphere);
            break;
    }
    return object;
}


clMaterial to_clMaterial(const Material& _material) {
    clMaterial material;
    material.color = {_material.color[0], _material.color[1], _material.color[2], 1.0f};
    return material;
}


clScene to_clScene(const Scene& _scene) {
    clScene scene;
    scene.num_objects = _scene.num_objects;
    assert(scene.num_objects <= MAX_OBJECTS);
    for (int i = 0; i < MAX_OBJECTS; i++) {
        scene.objects[i] = to_clObject(_scene.objects[i]);
    }
    for (int i = 0; i < MAX_MATERIALS; i++) {
        scene.materials[i] = to_clMaterial(_scene.materials[i]);
    }
    scene.sky_color = {_scene.sky_color[0], _scene.sky_color[1], _scene.sky_color[2], 1.0f};
    return scene;
}

}
