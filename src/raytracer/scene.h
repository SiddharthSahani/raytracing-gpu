
#pragma once

#include "src/raytracer/material.h"
#include "src/raytracer/objects.h"
#include <vector>


namespace rt::internal {

struct Object {
    union {
        Sphere sphere;
        Triangle triangle;
    };
    cl_uint type;
    cl_uint materialIndex;
};


Object convert(const rt::Object& _object) {
    Object object;
    if (auto ptr = std::get_if<Sphere>(&_object.internal)) {
        object.sphere = *ptr;
        object.type = 0;
    } else if (auto ptr = std::get_if<Triangle>(&_object.internal)) {
        object.triangle = *ptr;
        object.type = 1;
    } else {
        printf("ERROR: While convert rt::Object to rt::internal::Object\n");
    }
    return object;
}

}


namespace rt {

struct Scene {
    std::vector<Object> objects;
    glm::vec3 backgroundColor;
};

}
