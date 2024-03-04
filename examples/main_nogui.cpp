
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "src/raytracer.h"
#include "src/raytracer/camera.h"
#include "src/scene_loader.h"
#include <argparse/argparse.hpp>
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
    argparse::ArgumentParser parser("rt");
    parser.add_argument("sceneFile")
        .help("Scene json file to load");
    parser.add_argument("outFile")
        .help("The output image file")
        .default_value("output.png");
    parser.add_argument("-w", "--imageWidth").
        help("Image width")
        .default_value(1280u)
        .scan<'u', uint32_t>();
    parser.add_argument("-h", "--imageHeight").
        help("Image height")
        .default_value(720u)
        .scan<'u', uint32_t>();
    parser.add_argument("-s", "--sampleCount").
        help("Number of samples per pixel")
        .default_value(1024u)
        .scan<'u', uint32_t>();
    parser.add_argument("", "--clPlatformIdx").
        help("Index of preferred opencl platform")
        .default_value(0u)
        .scan<'u', uint32_t>();
    parser.add_argument("", "--clDeviceIdx").
        help("Index of preferred opencl device")
        .default_value(0u)
        .scan<'u', uint32_t>();

    try {
        parser.parse_args(argc, argv);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        exit(1);
    }

    const std::string sceneFile = parser.get<std::string>("sceneFile");
    const std::string outFile = parser.get<std::string>("outFile");
    const uint32_t imageWidth = parser.get<uint32_t>("imageWidth");
    const uint32_t imageHeight = parser.get<uint32_t>("imageHeight");
    const uint32_t sampleCount = parser.get<uint32_t>("sampleCount");
    const uint32_t clPlatformIdx = parser.get<uint32_t>("clPlatformIdx");
    const uint32_t clDeviceIdx = parser.get<uint32_t>("clDeviceIdx");

    cl::Platform platform = rt::getAllClPlatforms()[clPlatformIdx];
    cl::Device device = rt::getAllClDevices(platform)[clDeviceIdx];
    rt::CL_Objects clObj = rt::createClObjects(platform, device);

    rt::Raytracer raytracer({imageWidth, imageHeight}, clObj, rt::Format::RGBA8, false);

    bool success;
    rt::Scene scene = rt::loadScene(sceneFile.c_str(), &success);
    if (success) {
        printf("'%s' loaded successfully\n", sceneFile.c_str());
    } else {
        printf("'%s' failed to load\n", sceneFile.c_str());
        return 1;
    }

    auto camera = rt::createCamera(60.0f, {imageWidth, imageHeight}, {0, 0, 6}, {0, 0, -1});
    auto _scene = rt::convert(scene, clObj.context, clObj.queue);

    RT_TIME_STMT("Time taken to compile cl prog:", raytracer.createClKernels({.sampleCount = sampleCount, .bounceLimit = 5}));
    RT_TIME_STMT("Time taken to render:", raytracer.renderScene(_scene, camera, {.sampleCount = sampleCount, .bounceLimit = 5}));

    printf("Image saved: %s\n", raytracer.saveAsImage(outFile.c_str()) ? "true" : "false");
}
