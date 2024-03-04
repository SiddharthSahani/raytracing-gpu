
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


rt::Scene createScene_6() {
    auto pinkMat = rt::createMaterial({1.0f, 0.0f, 1.0f}, 0.0f);
    auto blueMat = rt::createMaterial({0.2f, 0.3f, 1.0f}, 0.0f);
    auto brownMat = rt::createMaterial({0.8f, 0.5f, 0.2f}, 0.0f);
    auto mirrorMat = rt::createMaterial({0.8f, 0.8f, 0.8f}, 1.0f);

    auto sph1 = rt::createSphere({0.0f, 0.0f, 0.0f}, 1.0f, pinkMat);
    auto sph2 = rt::createSphere({2.0f, 0.0f, 0.0f}, 1.0f, brownMat);
    auto ground = rt::createSphere({0.0f, -101.0f, 0.0f}, 100.0f, blueMat);
    auto triangle = rt::createTriangle({-1.0f, -1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f}, {-1.5f, 0.0f, 2.0f}, mirrorMat);

    rt::Scene scene;

    scene.objects.push_back(sph1);
    scene.objects.push_back(sph2);
    scene.objects.push_back(ground);
    scene.objects.push_back(triangle);

    scene.backgroundColor = {230, 230, 250};
    scene.backgroundColor /= 255.0f;

    return scene;
}


rt::Scene createScene_7() {
    auto greenMat = rt::createMaterial({0.2f, 0.6f, 0.0f}, 0.0f);
    auto blueMat = rt::createMaterial({0.2f, 0.6f, 1.0f}, 0.0f);
    auto mirrorMat = rt::createMaterial({0.7f, 0.7f, 0.7f}, 0.95f);

    auto sph1 = rt::createSphere({0.0f, 0.0f, 0.0f}, 1.0f, blueMat);
    auto ground = rt::createSphere({0.0f, -101.0f, 0.0f}, 100.0f, greenMat);
    auto tri1 = rt::createTriangle({-2.0f, -1.0f, 2.0f}, {-2.0f, -1.0f, -2.0f}, {2.0f, -0.5f, -2.0f}, mirrorMat);
    auto tri2 = rt::createTriangle({2.0f, -0.5f, -2.0f}, {2.0f, -0.5f, 2.0f}, {-2.0f, -1.0f, 2.0f}, mirrorMat);

    rt::Scene scene;

    scene.objects.push_back(sph1);
    scene.objects.push_back(ground);
    scene.objects.push_back(tri1);
    scene.objects.push_back(tri2);

    scene.backgroundColor = {104, 184, 235};
    scene.backgroundColor /= 255.0f;

    return scene;
}


rt::Scene createScene_8() {
    auto pinkMat = rt::createMaterial({1.0f, 0.0f, 1.0f}, 0.0f);
    auto blueMat = rt::createMaterial({0.2f, 0.3f, 1.0f}, 0.0f);
    auto lightMat = rt::createEmissiveMaterial({0.8f, 0.5f, 0.2f}, 100.0f);
    auto mirrorMat = rt::createMaterial({0.8f, 0.8f, 0.8f}, 1.0f);

    auto sph = rt::createSphere({0.5f, 0.0f, 0.0f}, 1.0f, pinkMat);
    auto ground = rt::createSphere({0.0f, -101.0f, 0.0f}, 100.0f, blueMat);
    auto light = rt::createSphere({32.0f, 4.0f, -32.0f}, 20.0f, lightMat);
    auto tri1 = rt::createTriangle({0.0f, 2.0f, -3.0f}, {-2.0f, 2.0f, -2.0f}, {0.0f, -1.0f, -3.0f}, mirrorMat);
    auto tri2 = rt::createTriangle({-2.0f, 2.0f, -2.0f}, {0.0f, -1.0f, -3.0f}, {-2.0f, -1.0f, -2.0f}, mirrorMat);

    rt::Scene scene;

    scene.objects.push_back(sph);
    scene.objects.push_back(ground);
    scene.objects.push_back(light);
    scene.objects.push_back(tri1);
    scene.objects.push_back(tri2);

    scene.backgroundColor = {70, 70, 70};
    scene.backgroundColor /= 255.0f;

    return scene;
}


std::vector<rt::Scene> createAllScenes() {
    std::vector<rt::Scene> scenes = {
        createScene_1(),
        createScene_2(),
        createScene_3(),
        createScene_4(),
        createScene_5(),
        createScene_6(),
        createScene_7(),
        createScene_8(),
    };
    return scenes;
}
