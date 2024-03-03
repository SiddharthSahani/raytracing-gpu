
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "src/raytracer.h"
#include "src/raytracer/camera.h"
#include "src/scene_loader.h"
#include <chrono>

using namespace std::chrono;


#define RT_TIME_STMT(name, statement)                   \
{                                                       \
    auto startTime = high_resolution_clock::now();      \
    statement;                                          \
    auto stopTime = high_resolution_clock::now();       \
    auto timeTaken_ns = (stopTime - startTime).count(); \
    printf(                                             \
        name " %f secs (%f ms)\n",                 \
        (float) timeTaken_ns / 1'000'000'000,           \
        (float) timeTaken_ns / 1'000'000                \
    );                                                  \
}


int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("Provide scene json file as an argument\n");
        return 1;
    }

    // to select preffered gpu
    const int clPlatformIdx = 0;
    const int clDeviceIdx = 0;
    // window and image size
    const int imageWidth = 1280;
    const int imageHeight = 720;
    // number of samples per pixel
    const int sampleCount = 1024;

    cl::Platform platform = rt::getAllClPlatforms()[clPlatformIdx];
    cl::Device device = rt::getAllClDevices(platform)[clDeviceIdx];
    rt::CL_Objects clObj = rt::createClObjects(platform, device);

    rt::Raytracer raytracer({imageWidth, imageHeight}, clObj, rt::Format::RGBA8, false);

    bool success;
    rt::Scene scene = rt::loadScene(argv[1], &success);
    if (success) {
        printf("'%s' loaded successfully\n", argv[1]);
    } else {
        printf("'%s' failed to load\n", argv[1]);
        return 1;
    }

    auto camera = rt::createCamera(60.0f, {imageWidth, imageHeight}, {0, 0, 6}, {0, 0, -1});
    auto _scene = rt::convert(scene, clObj.context, clObj.queue);

    RT_TIME_STMT("Time taken to compile cl prog:", raytracer.createClKernels({.sampleCount = sampleCount, .bounceLimit = 5}));
    RT_TIME_STMT("Time taken to render:", raytracer.renderScene(_scene, camera, {.sampleCount = sampleCount, .bounceLimit = 5}));

    printf("Image saved: %s\n", raytracer.saveAsImage("test.png") ? "true" : "false");
}
