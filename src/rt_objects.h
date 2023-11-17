
#pragma once

#include <glm/glm.hpp>
#include <CL/opencl.hpp>

#define MAX_OBJECTS 5
#define MAX_MATERIALS 5


namespace rt {

#define OBJECT_TYPE_SPHERE 0
#define OBJECT_TYPE_TRIANGLE 1


struct Sphere {
    glm::vec3 position;
    float radius;
};


struct Triangle {
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;
};


struct RendererConfig {
    cl_uint samples;
    cl_uint bounces;
};


struct Object {
    union {
        Sphere sphere;
        Triangle triangle;
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
    cl_uint num_objects;
    cl_float3 sky_color;
    clObject objects[MAX_OBJECTS];
    clMaterial materials[MAX_MATERIALS];
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
    scene.num_objects = _scene.num_objects;
    assert(scene.num_objects <= MAX_OBJECTS);
    for (int i = 0; i < MAX_OBJECTS; i++) {
        scene.objects[i] = to_clObject(_scene.objects[i]);
    }
    for (int i = 0; i < MAX_MATERIALS; i++) {
        scene.materials[i] = to_clMaterial(_scene.materials[i]);
    }
    scene.sky_color = {_scene.sky_color.x, _scene.sky_color.y, _scene.sky_color.z, 1.0f};
    return scene;
}

}
