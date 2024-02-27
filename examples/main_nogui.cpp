
#include "src/raytracer.h"
#include "src/raytracer/camera.h"
#include "src/test_scenes.h"
#include <chrono>

using namespace std::chrono;


#define RT_TIME_STMT(name, statement)                   \
{                                                       \
    auto startTime = high_resolution_clock::now();      \
    statement;                                          \
    auto stopTime = high_resolution_clock::now();       \
    auto timeTaken_ns = (stopTime - startTime).count(); \
    printf(                                             \
        name " took %f secs (%f ms)\n",                 \
        (float) timeTaken_ns / 1'000'000'000,           \
        (float) timeTaken_ns / 1'000'000                \
    );                                                  \
}



int main() {
    const int clPlatformIdx = 0;
    const int clDeviceIdx = 0;
    const int imageWidth = 1280;
    const int imageHeight = 720;
    const int sampleCount = 1024;

    cl::Platform platform = rt::getAllClPlatforms()[clPlatformIdx];
    cl::Device device = rt::getAllClDevices(platform)[clDeviceIdx];
    rt::CL_Objects clObj = rt::createClObjects(platform, device);

    rt::Raytracer raytracer({imageWidth, imageHeight}, clObj, rt::Format::RGBA8, false);

    auto camera = rt::createCamera(60.0f, {imageWidth, imageHeight}, {0, 0, 6}, {0, 0, -1});
    auto allScenes = createAllScenes(clObj.context, clObj.queue);
    auto scene = allScenes[7];

    RT_TIME_STMT("Time taken to compile cl prog: ", raytracer.createClKernels({.sampleCount = sampleCount, .bounceLimit = 5}));
    RT_TIME_STMT("Time taken to render: ", raytracer.renderScene(scene, camera, {.sampleCount = sampleCount, .bounceLimit = 5}));

    printf("Image saved: %d\n", raytracer.saveAsImage("test.png"));
}
