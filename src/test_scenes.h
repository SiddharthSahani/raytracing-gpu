
#pragma once

#include "src/rt_objects.h"


rt::Object create_sphere(glm::vec3 position, float radius) {
    rt::Object object;
    object.sphere = {.position = position, .radius = radius};
    object.type = OBJECT_TYPE_SPHERE;
    return object;
}


rt::Object create_triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2) {
    rt::Object object;
    object.triangle = {.v0 = v0, .v1 = v1, .v2 = v2};
    object.type = OBJECT_TYPE_TRIANGLE;
    return object;
}


rt::clScene create_scene_1() {
    rt::Scene scene;

    scene.objects[0] = create_sphere({0.0f, 0.0f, 0.0f}, 1.0f);
    scene.objects[0].material_idx = 0;
    scene.objects[1] = create_sphere({0.0f, -6.0f, 0.0f}, 5.0f);
    scene.objects[1].material_idx = 1;

    scene.materials[0] = {.color = {0.2f, 0.9f, 0.8f}};
    scene.materials[1] = {.color = {1.0f, 0.0f, 1.0f}};

    scene.num_objects = 2;

    scene.sky_color = {210.0f, 210.0f, 230.0f};
    scene.sky_color /= 255.0f;

    return rt::to_clScene(scene);
}


rt::clScene create_scene_2() {
    rt::Scene scene;

    scene.objects[0] = create_sphere({0.0f, 0.0f, 0.0f}, 1.0f);
    scene.objects[0].material_idx = 0;
    scene.objects[1] = create_triangle({-2.0f, -1.0f, -1.5f}, {2.0f, -1.0f, -1.5f}, {0.0f, -1.5f, 2.0f});
    scene.objects[1] = create_triangle({-2.0f, -1.0f, -1.5f}, {2.0f, -1.0f, -1.5f}, {0.0f, -1.0f, 2.0f});
    scene.objects[1].material_idx = 1;

    scene.materials[0] = {.color = {0.0f, 0.2f, 0.8f}};
    scene.materials[1] = {.color = {0.0f, 1.0f, 1.0f}};

    scene.num_objects = 2;

    scene.sky_color = {180.0f, 150.0f, 200.0f};
    scene.sky_color /= 255.0f;

    return rt::to_clScene(scene);
}


rt::clScene create_scene_3() {
    rt::Scene scene;

    scene.objects[0] = create_sphere({0.0f, 0.0f, 0.0f}, 1.0f);
    scene.objects[0].material_idx = 0;
    scene.objects[1] = create_sphere({2.0f, 0.0f, 0.0f}, 1.0f);
    scene.objects[1].material_idx = 1;
    scene.objects[2] = create_sphere({0.0f, -101.0f, 0.0f}, 100.0f);
    scene.objects[2].material_idx = 2;

    scene.materials[0] = {.color = {1.0f, 0.0f, 1.0f}};
    scene.materials[1] = {.color = {1.0f, 0.0f, 0.0f}};
    scene.materials[2] = {.color = {0.3f, 0.8f, 0.3f}};

    scene.num_objects = 3;

    scene.sky_color = {225.0f, 225.0f, 255.0f};
    scene.sky_color /= 255.0f;

    return rt::to_clScene(scene);
}
