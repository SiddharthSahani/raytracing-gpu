
#pragma once

#include "materials.h"
#include "objects.h"

const int SCENE_MAX_OBJECTS = 5;
const int SCENE_MAX_MATERIALS = 3;


namespace rt::internal {

struct Object {
    union {
        Sphere sphere;
        Triangle triangle;
    };
    cl_uint type;
    cl_uint material_idx;
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

struct CompiledScene {
    internal::Object objects[SCENE_MAX_OBJECTS];
    Material materials[SCENE_MAX_MATERIALS];
    cl_float3 sky_color;
    cl_uint num_objects;
};


class Scene {

    public:
        void addObject(const Object& object) { m_objects.push_back(object); }
        void setSkyColor(const glm::vec3& color) { m_sky_color = color; }
        CompiledScene compile(bool* success) const;
        void printInfo() const;

    private:
        std::vector<Object> m_objects;
        glm::vec3 m_sky_color;

};


CompiledScene Scene::compile(bool* success) const {
    bool valid = true;

    // number of objects/materials exceeds the limit
    if (m_objects.size() > SCENE_MAX_OBJECTS) {
        printf("ERRROR: Number of objects (%d) exceeds max object count (%d)\n", m_objects.size(), SCENE_MAX_OBJECTS);
        valid = false;
    }
    if (m_objects.size() > SCENE_MAX_MATERIALS) {
        printf("ERROR: Number of  materials (%d) exceeds max material count (%d)\n", m_objects.size(), SCENE_MAX_MATERIALS);
        valid = false;
    }

    CompiledScene scene;
    scene.num_objects = 0;

    if (valid) {
        for (int i = 0; i < m_objects.size(); i++) {
            scene.objects[i] = internal::convert(m_objects[i]);
            scene.objects[i].material_idx = i;
        }
        for (int i = 0; i < m_objects.size(); i++) {
            scene.materials[i] = *m_objects[i].material;
        }
        scene.sky_color = {m_sky_color.r, m_sky_color.g, m_sky_color.b, 1.0f};
        scene.num_objects = m_objects.size();
    }

    if (success) 
        *success = valid;
    return scene;
}


void Scene::printInfo() const {
    printf("Number of Objects: %d\n", m_objects.size());

    for (int i = 0; i < m_objects.size(); i++) {
        auto& obj = m_objects[i];
        auto& mat = obj.material;

        printf("Object[%d]:\n", i);
        if (auto ptr = std::get_if<Sphere>(&obj.internal)) {
            printSphereInfo(*ptr);
        } else if (auto ptr = std::get_if<Triangle>(&obj.internal)) {
            printTriangleInfo(*ptr);
        }

        printMaterialInfo(*mat);
    }

    printf("Sky color: %f %f %f\n", m_sky_color.r, m_sky_color.g, m_sky_color.b);
}

}
