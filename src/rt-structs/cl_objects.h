
#pragma once

#include "objects.h"
#include <CL/opencl.hpp>


namespace rt {

struct clSphere {
    cl_float3 position;
    cl_float radius;
};


struct clTriangle {
    cl_float3 v0;
    cl_float3 v1;
    cl_float3 v2;
};


struct clObject {
    union {
        clSphere sphere;
        clTriangle triangle;
    };
    cl_uint type;
    cl_uint material_idx;
};


struct clMaterial {
    cl_float3 color;
};


struct clScene {
    clObject objects[MAX_OBJECTS];
    clMaterial materials[MAX_MATERIALS];
    cl_float3 sky_color;
    cl_uint object_count;
};


struct clRendererConfig {
    cl_uint sample_count;
    cl_uint bounce_limit;
};


clSphere to_clSphere(const Sphere& _sphere) {
    clSphere sphere;
    sphere.position = {_sphere.position.x, _sphere.position.y, _sphere.position.z, 1.0f};
    sphere.radius = _sphere.radius;
    return sphere;
}


clTriangle to_clTriangle(const Triangle& _triangle) {
    clTriangle triangle;
    triangle.v0 = {_triangle.v0.x, _triangle.v0.y, _triangle.v0.z, 1.0f};
    triangle.v1 = {_triangle.v1.x, _triangle.v1.y, _triangle.v1.z, 1.0f};
    triangle.v2 = {_triangle.v2.x, _triangle.v2.y, _triangle.v2.z, 1.0f};
    return triangle;
}


clObject to_clObject(const Object& _object) {
    clObject object;
    object.type = _object.type;
    object.material_idx = _object.material_idx;
    switch (_object.type) {
        case OBJECT_TYPE_SPHERE:
            object.sphere = to_clSphere(_object.sphere);
            break;
        case OBJECT_TYPE_TRIANGLE:
            object.triangle = to_clTriangle(_object.triangle);
            break;
    }
    return object;
}


clMaterial to_clMaterial(const Material& _material) {
    clMaterial material;
    material.color = {_material.color.x, _material.color.y, _material.color.z, 1.0f};
    return material;
}


clScene to_clScene(const Scene& _scene) {
    clScene scene;
    scene.object_count = _scene.object_count;
    assert(scene.object_count <= MAX_OBJECTS);

    for (int i = 0; i < MAX_OBJECTS; i++) {
        scene.objects[i] = to_clObject(_scene.objects[i]);
    }
    for (int i = 0; i < MAX_MATERIALS; i++) {
        scene.materials[i] = to_clMaterial(_scene.materials[i]);
    }
    scene.sky_color = {_scene.sky_color.x, _scene.sky_color.y, _scene.sky_color.z, 1.0f};
    return scene;
}


clRendererConfig to_clRendererConfig(const RendererConfig& _config) {
    clRendererConfig config;
    config.sample_count = _config.sample_count;
    config.bounce_limit = _config.bounce_limit;
    return config;
}

}
