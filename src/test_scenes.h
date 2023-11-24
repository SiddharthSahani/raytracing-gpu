
#pragma once

#include "scene.h"


rt::CompiledScene create_scene_1() {
    auto cyan_mat = rt::createMaterial({0.2f, 0.9f, 0.8f}, 0.3f);
    auto pink_mat = rt::createMaterial({1.0f, 0.0f, 1.0f}, 0.7f);

    auto sph1 = rt::createSphere({0.0f, 0.0f, 0.0f}, 1.0f, cyan_mat);
    auto sph2 = rt::createSphere({0.0f, -6.0f, 0.0f}, 5.0f, pink_mat);

    rt::Scene scene;
    scene.addObject(sph1);
    scene.addObject(sph2);
    scene.setSkyColor(glm::vec3(210, 210, 230) / 255.0f);

    return scene.compile(nullptr);
}


rt::CompiledScene create_scene_2() {
    auto blue_diff_mat = rt::createMaterial({0.0f, 0.2f, 0.8f}, 0.0f);
    auto cyan_refl_mat = rt::createMaterial({0.0f, 1.0f, 1.0f}, 1.0f);

    auto sph = rt::createSphere({0.0f, 0.0f, 0.0f}, 1.0f, blue_diff_mat);
    auto tri = rt::createTriangle({-2.0f, -1.0f, -1.5f}, {2.0f, -1.0f, -1.5f}, {0.0f, -1.0f, 2.0f}, cyan_refl_mat);

    rt::Scene scene;
    scene.addObject(sph);
    scene.addObject(tri);
    scene.setSkyColor(glm::vec3(180, 150, 200) / 255.0f);

    return scene.compile(nullptr);
}


rt::CompiledScene create_scene_3() {
    auto purple_mat = rt::createMaterial({1.0f, 0.0f, 1.0f}, 0.5f);
    auto red_mat = rt::createMaterial({1.0f, 0.0f, 0.0f}, 0.6f);
    auto green_mat = rt::createMaterial({0.3f, 0.8f, 0.3f}, 0.2f);

    auto sph1 = rt::createSphere({0.0f, 0.0f, 0.0f}, 1.0f, purple_mat);
    auto sph2 = rt::createSphere({2.0f, 0.0f, 0.0f}, 1.0f, red_mat);
    auto sph3 = rt::createSphere({0.0f, -101.0f, 0.0f}, 100.0f, green_mat);

    rt::Scene scene;
    scene.addObject(sph1);
    scene.addObject(sph2);
    scene.addObject(sph3);
    scene.setSkyColor(glm::vec3(225, 225, 255) / 255.0f);

    return scene.compile(nullptr);
}
