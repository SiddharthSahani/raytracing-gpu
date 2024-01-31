
#define RT_PRINT_LOG

#include "src/raytracer.cpp"
#include "src/renderers/raylibrenderer.cpp"
#include "src/test_scenes.h"


bool isSceneChanged() {
    return IsKeyDown(KEY_C) && (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT));
}


int getSceneIndex(int sceneCount) {
    static int sceneIndex = 0;
    if (IsKeyDown(KEY_C)) {
        sceneIndex += IsKeyPressed(KEY_RIGHT);
        sceneIndex -= IsKeyPressed(KEY_LEFT);
    }
    return ((sceneIndex % sceneCount) + sceneCount) % sceneCount;
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
    const int displayUpdatesPerSec = 1;
    const int kernelExecsPerSec = 30;
    // raytracing config
    const rt::Config rtConfig = {
        .sampleCount = 16, .bounceLimit = 5
    };

    cl::Platform platform = rt::getAllClPlatforms()[clPlatformIdx];
    cl::Device device = rt::getAllClDevices(platform)[clDeviceIdx];
    rt::CL_Objects clObj = rt::createClObjects(platform, device);

    rt::Raytracer raytracer({imageWidth, imageHeight}, clObj, rt::Format::RGBA32F, true);
    rt::RaylibRenderer renderer(raytracer, {displayWidth, displayHeight}, kernelExecsPerSec);

    auto camera = rt::createCamera(60.0f, {imageWidth, imageHeight}, {0, 0, 6}, {0, 0, -1});
    auto scenes = createAllScenes(clObj.context, clObj.queue);
    int numScenes = scenes.size();

    int sceneIdx = 0;
    raytracer.renderScene(scenes[sceneIdx], camera, rtConfig);

    int displayUpdateCount = 0;
    int kernelExecCount = -1;

    while (!WindowShouldClose()) {
        if (isSceneChanged()) {
            raytracer.resetFrameCount();
            sceneIdx = getSceneIndex(numScenes);
            displayUpdateCount = 0;
            kernelExecCount = 0;
        }

        kernelExecCount++;
        raytracer.renderScene(scenes[sceneIdx], camera, rtConfig);
        raytracer.accumulatePixels();

        if (kernelExecCount % (kernelExecsPerSec / displayUpdatesPerSec) == 0 || isSceneChanged()) {
            displayUpdateCount++;
            renderer.update();
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        renderer.draw();
        DrawText(TextFormat("Kernel Exec Count: %d", kernelExecCount), 10, 10, 18, GREEN);
        DrawText(TextFormat("Display Update Count: %d", displayUpdateCount), 10, 30, 18, GREEN);
        DrawText(TextFormat("Samples per pixel: %d", rtConfig.sampleCount * kernelExecCount), 10, 50, 18, GREEN);
        DrawFPS(10, 70);
        EndDrawing();
    }
}
