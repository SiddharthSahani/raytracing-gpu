
#include "src/raytracer.h"
#include "src/backend/raylib/renderer.h"
#include "src/backend/raylib/camera.h"
#include "src/test_scenes.h"


bool isSceneChanged() {
    return rl::IsKeyDown(rl::KEY_S) && (rl::IsKeyPressed(rl::KEY_LEFT) || rl::IsKeyPressed(rl::KEY_RIGHT));
}


int getSceneIndex(int sceneCount) {
    static int sceneIndex = 0;
    if (rl::IsKeyDown(rl::KEY_S)) {
        sceneIndex += rl::IsKeyPressed(rl::KEY_RIGHT);
        sceneIndex -= rl::IsKeyPressed(rl::KEY_LEFT);
    }
    return ((sceneIndex % sceneCount) + sceneCount) % sceneCount;
}


int getConfigIndex(int configCount) {
    static int configIndex = 0;
    if (rl::IsKeyDown(rl::KEY_C)) {
        configIndex += rl::IsKeyPressed(rl::KEY_RIGHT);
        configIndex -= rl::IsKeyPressed(rl::KEY_LEFT);
    }
    return ((configIndex % configCount) + configCount) % configCount;
}


int main(int argc, char* argv[]) {
    // to select preffered gpu
    const int clPlatformIdx = 0;
    const int clDeviceIdx = 0;
    // window and image size
    const int displayWidth = 1280;
    const int displayHeight = 720;
    const float scale = 2.0f;
    const int imageWidth = displayWidth / scale;
    const int imageHeight = displayHeight / scale;
    // kernel and display timers
    const int displayUpdatesPerSec = 30;
    const int kernelExecsPerSec = 30; // this also acts as FPS

    rl::SetTraceLogLevel(rl::LOG_WARNING);
    rl::InitWindow(displayWidth, displayHeight, "Raytracing [backend: raylib]");

    cl::Platform platform = rt::getAllClPlatforms()[clPlatformIdx];
    cl::Device device = rt::getAllClDevices(platform)[clDeviceIdx];
    rt::CL_Objects clObj;

    if (rt::supports_clGlInterop(device)) {
        printf("Creating cl context with gl-interop\n");
        clObj = rt::createClObjects_withInterop(platform, device);
    } else {
        printf("Creating normal cl context since gl-interop is not supported\n");
        clObj = rt::createClObjects(platform, device);
    }

    rt::Raytracer raytracer({imageWidth, imageHeight}, clObj, rt::Format::RGBA32F, true);
    rt::Renderer renderer(raytracer, {displayWidth, displayHeight}, kernelExecsPerSec);

    auto camera = rt::Camera(60.0f, {imageWidth, imageHeight}, {0, 0, 6}, {0, 0, -1}, {});
    auto scenes = createAllScenes(clObj.context, clObj.queue);
    int numScenes = scenes.size();

    rt::Config configs[] = {
        {.sampleCount =  4, .bounceLimit = 5},
        {.sampleCount = 16, .bounceLimit = 5},
        {.sampleCount = 32, .bounceLimit = 5},
    };

    for (int i = 0; i < sizeof(configs) / sizeof(rt::Config); i++) {
        raytracer.createClKernels(configs[i]);
    }

    int sceneIdx = 0;
    int configIdx = 0;
    int displayUpdateCount = 0;
    int kernelExecCount = 0;

    while (!rl::WindowShouldClose()) {
        if (camera.update(rl::GetFrameTime()) || isSceneChanged()) {
            raytracer.resetFrameCount();
            sceneIdx = getSceneIndex(numScenes);
            displayUpdateCount = 0;
            kernelExecCount = 0;
        }
        configIdx = getConfigIndex(sizeof(configs) / sizeof(rt::Config));

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
