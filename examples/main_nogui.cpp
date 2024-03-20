
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


struct CommandLineOptions {
    std::string sceneFile;
    std::string outFile;
    uint32_t imageWidth;
    uint32_t imageHeight;
    uint32_t sampleCount;
    uint32_t clPlatformIdx;
    uint32_t clDeviceIdx;
};


CommandLineOptions parseCommandLine(int argc, char* argv[]) {
    argparse::ArgumentParser parser("raytracing-nogui", "0.2.0");
    parser.add_argument("sceneFile")
        .help("Scene json file to load");
    parser.add_argument("-o", "--outFile")
        .help("The output image file")
        .default_value("output.png");
    parser.add_argument("-w", "--imageWidth")
        .help("Image width")
        .default_value(1280u)
        .scan<'u', uint32_t>();
    parser.add_argument("-h", "--imageHeight")
        .help("Image height")
        .default_value(720u)
        .scan<'u', uint32_t>();
    parser.add_argument("-s", "--sampleCount")
        .help("Number of samples per pixel")
        .default_value(1024u)
        .scan<'u', uint32_t>();
    parser.add_argument("", "--clPlatformIdx")
        .help("Index of preferred opencl platform")
        .default_value(0u)
        .scan<'u', uint32_t>();
    parser.add_argument("", "--clDeviceIdx")
        .help("Index of preferred opencl device")
        .default_value(0u)
        .scan<'u', uint32_t>();

    try {
        parser.parse_args(argc, argv);
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        exit(1);
    }

    CommandLineOptions options = {
        .sceneFile = parser.get<std::string>("sceneFile"),
        .outFile = parser.get<std::string>("outFile"),
        .imageWidth = parser.get<uint32_t>("imageWidth"),
        .imageHeight = parser.get<uint32_t>("imageHeight"),
        .sampleCount = parser.get<uint32_t>("sampleCount"),
        .clPlatformIdx = parser.get<uint32_t>("clPlatformIdx"),
        .clDeviceIdx = parser.get<uint32_t>("clDeviceIdx")
    };
    return options;
}


int main(int argc, char* argv[]) {
    const CommandLineOptions options = parseCommandLine(argc, argv);
    const std::string sceneFile = options.sceneFile;
    const std::string outFile = options.outFile;
    const uint32_t imageWidth = options.imageWidth;
    const uint32_t imageHeight = options.imageHeight;
    const uint32_t sampleCount = options.sampleCount;
    const uint32_t clPlatformIdx = options.clPlatformIdx;
    const uint32_t clDeviceIdx = options.clDeviceIdx;

    bool success;
    rt::Scene scene = rt::loadScene(sceneFile.c_str(), &success);
    if (success) {
        printf("'%s' loaded successfully\n", sceneFile.c_str());
        printf("Number of objects in scene: %d\n", scene.objects.size());
    } else {
        printf("'%s' failed to load\n", sceneFile.c_str());
        return 1;
    }

    cl::Platform platform = rt::getAllClPlatforms()[clPlatformIdx];
    cl::Device device = rt::getAllClDevices(platform)[clDeviceIdx];
    rt::CL_Objects clObj = rt::createClObjects(platform, device);

    rt::Raytracer raytracer({imageWidth, imageHeight}, clObj, rt::Format::RGBA8, false);
    auto camera = rt::createCamera(60.0f, {imageWidth, imageHeight}, {0, 0, 6}, {0, 0, -1});
    auto _scene = rt::convert(scene, clObj.context, clObj.queue);

    RT_TIME_STMT("Time taken to render:", raytracer.renderScene(_scene, camera, {.sampleCount = sampleCount, .bounceLimit = 5}));
    RT_TIME_STMT("Time taken to save image:", printf("Image saved: %s\n", raytracer.saveAsImage(outFile.c_str()) ? "true" : "false"));
}
