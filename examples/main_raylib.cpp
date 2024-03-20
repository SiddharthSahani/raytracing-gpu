
#include "src/raytracer.h"
#include "src/backend/raylib/renderer.h"
#include "src/backend/raylib/camera.h"
#include "src/test_scenes.h"
#include "src/scene_loader.h"
#include <argparse/argparse.hpp>


bool isSceneChanged() {
    return rl::IsKeyDown(rl::KEY_S) && (rl::IsKeyPressed(rl::KEY_LEFT) || rl::IsKeyPressed(rl::KEY_RIGHT));
}


int getIndex(int current, int count) {
    current += rl::IsKeyPressed(rl::KEY_RIGHT);
    current -= rl::IsKeyPressed(rl::KEY_LEFT);
    return ((current % count) + count) % count;
}


struct CommandLineOptions {
    std::vector<std::string> sceneFiles;
    uint32_t displayWidth;
    uint32_t displayHeight;
    float scale;
    uint32_t kernelExecsPerSec;
    uint32_t clPlatformIdx;
    uint32_t clDeviceIdx;
};


CommandLineOptions parseCommandLine(int argc, char* argv[]) {
    argparse::ArgumentParser parser("raytracing-raylib", "0.2.0");
    parser.add_argument("sceneFiles")
        .help("Scene json file to load")
        .nargs(argparse::nargs_pattern::any);
    parser.add_argument("-w", "--displayWidth")
        .help("Display width")
        .default_value(1280u)
        .scan<'u', uint32_t>();
    parser.add_argument("-h", "--displayHeight")
        .help("Display height")
        .default_value(720u)
        .scan<'u', uint32_t>();
    parser.add_argument("-s", "--scale")
        .help("Scaling factor for image")
        .default_value(2.0f)
        .scan<'f', float>();
    parser.add_argument("-k", "--kernelExecsPerSec")
        .help("Number of kernel executions per second (also acts like FPS, minimum 30)")
        .default_value(30u)
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
        .sceneFiles = parser.get<std::vector<std::string>>("sceneFiles"),
        .displayWidth = parser.get<uint32_t>("displayWidth"),
        .displayHeight = parser.get<uint32_t>("displayHeight"),
        .scale = parser.get<float>("scale"),
        .kernelExecsPerSec = parser.get<uint32_t>("kernelExecsPerSec"),
        .clPlatformIdx = parser.get<uint32_t>("clPlatformIdx"),
        .clDeviceIdx = parser.get<uint32_t>("clDeviceIdx")
    };
    return options;
}


std::vector<rt::Scene> loadScenes(const std::vector<std::string>& sceneFiles) {
    std::vector<rt::Scene> scenes;
    
    bool success;
    for (const std::string& sceneFile : sceneFiles) {
        rt::Scene scene = rt::loadScene(sceneFile.c_str(), &success);
        if (success) {
            printf("Loaded '%s'\n", sceneFile.c_str());
            scenes.push_back(scene);
        } else {
            printf("'%s' failed to load\n", sceneFile.c_str());
        }
    }

    return scenes;
}


int main(int argc, char* argv[]) {
    const CommandLineOptions options = parseCommandLine(argc, argv);
    const std::vector<std::string>& sceneFiles = options.sceneFiles;
    const uint32_t displayWidth = options.displayWidth;
    const uint32_t displayHeight = options.displayHeight;
    const float scale = options.scale;
    const uint32_t kernelExecsPerSec = options.kernelExecsPerSec < 30 ? 30 : options.kernelExecsPerSec;
    const uint32_t clPlatformIdx = options.clPlatformIdx;
    const uint32_t clDeviceIdx = options.clDeviceIdx;

    const uint32_t textureWidth = displayWidth / scale;
    const uint32_t textureHeight = displayHeight / scale;
    const uint32_t displayUpdatesPerSec = 30;

    bool filesProvided = sceneFiles.size() > 0;
    std::vector<rt::Scene> _scenes = filesProvided ? loadScenes(sceneFiles) : createAllScenes();

    if (_scenes.size() == 0) {
        printf("Encountered problems loading files from arguments, exiting\n");
        exit(1);
    }

    rl::SetTraceLogLevel(rl::LOG_WARNING);
    rl::InitWindow(displayWidth, displayHeight, "Raytracing [backend: raylib]");

    cl::Platform platform = rt::getAllClPlatforms()[clPlatformIdx];
    cl::Device device = rt::getAllClDevices(platform)[clDeviceIdx];
    rt::CL_Objects clObj;

    bool clGlInterop = rt::supports_clGlInterop(device);
    if (clGlInterop) {
        printf("Creating cl context with gl-interop\n");
        clObj = rt::createClObjects_withInterop(platform, device);
    } else {
        printf("Creating normal cl context since gl-interop is not supported\n");
        clObj = rt::createClObjects(platform, device);
    }

    std::vector<rt::internal::Scene> scenes;
    for (const rt::Scene& scene : _scenes) {
        scenes.push_back(rt::convert(scene, clObj.context, clObj.queue));
    }

    rt::Format imgFormat = rt::Format::RGBA32F;
    rl::Texture outTexture = rt::createTexture({textureWidth, textureHeight}, imgFormat);
    rt::Raytracer raytracer({textureWidth, textureHeight}, clObj, imgFormat, true, clGlInterop ? outTexture.id : 0);
    rt::Renderer renderer(raytracer, {displayWidth, displayHeight}, kernelExecsPerSec, outTexture, clGlInterop);

    auto camera = rt::Camera(60.0f, {textureWidth, textureHeight}, {0, 0, 6}, {0, 0, -1}, {.speed = 10.0f});
    rt::Config configs[] = {
        {.sampleCount = 16, .bounceLimit = 5},
        {.sampleCount = 32, .bounceLimit = 5},
        {.sampleCount =  4, .bounceLimit = 5},
    };

    int sceneIdx = 0;
    int configIdx = 0;
    int displayUpdateCount = 0;
    int kernelExecCount = 0;

    while (!rl::WindowShouldClose()) {
        if (camera.update(rl::GetFrameTime()) || isSceneChanged()) {
            raytracer.resetFrameCount();
            sceneIdx = getIndex(sceneIdx, scenes.size());
            displayUpdateCount = 0;
            kernelExecCount = 0;
        }
        if (rl::IsKeyDown(rl::KEY_C)) {
            configIdx = getIndex(configIdx, sizeof(configs) / sizeof(rt::Config));
        }

        if (kernelExecCount % (kernelExecsPerSec / displayUpdatesPerSec) == 0 || isSceneChanged()) {
            displayUpdateCount++;
            renderer.update();
        }

        kernelExecCount++;
        raytracer.renderScene(scenes[sceneIdx], camera.getInternal(), configs[configIdx]);
        raytracer.accumulatePixels();

        rl::BeginDrawing();
        rl::ClearBackground(rl::ORANGE);
        renderer.draw();

        rl::DrawText(rl::TextFormat("Kernel Exec Count: %d", kernelExecCount), 10, 10, 18, rl::GREEN);
        rl::DrawText(rl::TextFormat("Display Update Count: %d", displayUpdateCount), 10, 30, 18, rl::GREEN);
        rl::DrawText(rl::TextFormat("Samples per pixel: %d", configs[configIdx].sampleCount * kernelExecCount), 10, 50, 18, rl::GREEN);
        rl::DrawText(rl::TextFormat("Current scene index: %d", sceneIdx), 10, 70, 18, rl::GREEN);
        rl::DrawText(rl::TextFormat("Config.sampleCount: %d", configs[configIdx].sampleCount), 10, 90, 18, rl::GREEN);
        rl::DrawFPS(10, 110);
        rl::EndDrawing();
    }
}
