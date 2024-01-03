
#pragma once

#include "src/raytracer/scene.h"


rt::Scene createScene_1() {
    auto cyanMat = rt::createMaterial({0.2f, 0.9f, 0.8f}, 0.3f);
    auto pinkMat = rt::createMaterial({1.0f, 0.0f, 1.0f}, 0.7f);

    auto sph1 = rt::createSphere({0.0f, 0.0f, 0.0f}, 1.0f, cyanMat);
    auto sph2 = rt::createSphere({0.0f, -6.0f, 0.0f}, 5.0f, pinkMat);

    rt::Scene scene;

    scene.objects.push_back(sph1);
    scene.objects.push_back(sph2);
    scene.backgroundColor = glm::vec3(210, 210, 230) / 255.0f;

    return scene;
}


rt::Scene createScene_2() {
    auto blueMat = rt::createMaterial({0.0f, 0.2f, 0.8f}, 0.0f);
    auto cyanMat = rt::createMaterial({0.0f, 1.0f, 1.0f}, 1.0f);

    auto sph = rt::createSphere({0.0f, 0.0f, 0.0f}, 1.0f, blueMat);
    auto tri = rt::createTriangle({-2.0f, -1.0f, -1.5f}, {2.0f, -1.0f, -1.5f}, {0.0f, -1.0f, 2.0f}, cyanMat);

    rt::Scene scene;

    scene.objects.push_back(sph);
    scene.objects.push_back(tri);
    scene.backgroundColor = glm::vec3(180, 150, 200) / 255.0f;

    return scene;
}


rt::Scene createScene_3() {
    auto purpleMat = rt::createMaterial({1.0f, 0.0f, 1.0f}, 0.5f);
    auto redMat = rt::createMaterial({1.0f, 0.0f, 0.0f}, 0.6f);
    auto geeenMat = rt::createMaterial({0.3f, 0.8f, 0.3f}, 0.2f);

    auto sph1 = rt::createSphere({0.0f, 0.0f, 0.0f}, 1.0f, purpleMat);
    auto sph2 = rt::createSphere({2.0f, 0.0f, 0.0f}, 1.0f, redMat);
    auto sph3 = rt::createSphere({0.0f, -101.0f, 0.0f}, 100.0f, geeenMat);

    rt::Scene scene;

    scene.objects.push_back(sph1);
    scene.objects.push_back(sph2);
    scene.objects.push_back(sph3);
    scene.backgroundColor = glm::vec3(225, 225, 255) / 255.0f;

    return scene;
}


rt::Scene createScene_4() {
    auto redMat = rt::createMaterial({1.0f, 0.3f, 0.3f}, 0.7f);
    auto greenMat = rt::createMaterial({0.3f, 1.0f, 0.3f}, 0.0f);
    auto greyMat = rt::createMaterial({0.6f, 0.6f, 0.6f}, 0.9f);

    auto sph1 = rt::createSphere({ 0.0f, 0.0f, -0.8f}, 0.9f, redMat);
    auto sph2 = rt::createSphere({-2.0f, 0.0f, 0.0f}, 0.7f, greyMat);
    auto sph3 = rt::createSphere({ 2.0f, 0.0f, 0.0f}, 0.7f, greyMat);
    auto ground = rt::createSphere({0.0f, -101.0f, 0.0f}, 100.0f, greenMat);

    rt::Scene scene;

    scene.objects.push_back(sph1);
    scene.objects.push_back(sph2);
    scene.objects.push_back(sph3);
    scene.objects.push_back(ground);
    scene.backgroundColor = glm::vec3(255, 255, 255) / 255.0f;

    return scene;
}


rt::Scene createScene_5() {
    auto groundMat = rt::createMaterial({0.8f, 0.8f, 0.0f}, 0.0f);
    auto centerMat = rt::createMaterial({0.7f, 0.3f, 0.3f}, 0.0f);
    auto sideMat = rt::createMaterial({0.8f, 0.8f, 0.8f}, 1.0f);
    auto lightMat = rt::createEmissiveMaterial({1.0f, 0.7f, 0.2f}, 10.0f);

    auto sph1 = rt::createSphere({ 0.0f, 0.0f, 3.5f}, 0.5f, centerMat);
    auto sph2 = rt::createSphere({-1.0f, 0.0f, 4.0f}, 0.5f, sideMat);
    auto sph3 = rt::createSphere({ 1.0f, 0.0f, 4.0f}, 0.5f, sideMat);
    auto ground = rt::createSphere({0.0f, -101.0f, -0.0f}, 100.5f, groundMat);
    auto light = rt::createSphere({0.0f, -0.4f, 4.8f}, 0.1f, lightMat);

    rt::Scene scene;

    scene.objects.push_back(sph1);
    scene.objects.push_back(sph2);
    scene.objects.push_back(sph3);
    scene.objects.push_back(ground);
    scene.objects.push_back(light);
    scene.backgroundColor = {0.3f, 0.3f, 0.4f};

    return scene;
}
