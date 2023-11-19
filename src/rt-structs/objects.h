
#pragma once

#include <glm/glm.hpp>

const int MAX_OBJECTS = 5;
const int MAX_MATERIALS = 3;
const int OBJECT_TYPE_SPHERE = 0;
const int OBJECT_TYPE_TRIANGLE = 1;


namespace rt {

struct Sphere {
    glm::vec3 position;
    float radius;
};


struct Triangle {
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;
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
    Object objects[MAX_OBJECTS];
    Material materials[MAX_MATERIALS];
    glm::vec3 sky_color;
    uint32_t object_count;
};


struct RendererConfig {
    uint32_t sample_count;
    uint32_t bounce_limit;
};

}
