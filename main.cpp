
#define RT_LOG printf
#include "src/renderer.h"
#include "src/test_scenes.h"
#include <chrono>

using namespace std::chrono;


#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define IMAGE_WIDTH (1280/2)
#define IMAGE_HEIGHT (720/2)


#define TIME_FUNCTION(name, func)                       \
{                                                       \
    auto startTime = high_resolution_clock::now();      \
    { func; }                                           \
    auto stopTime = high_resolution_clock::now();       \
    auto timeTaken_ns = (stopTime - startTime).count(); \
    printf(                                             \
        name " took %f secs (%f ms)\n",                 \
        (float) timeTaken_ns / 1'000'000'000,           \
        (float) timeTaken_ns / 1'000'000                \
    );                                                  \
}


std::vector<rt::CompiledScene> initializeScenes(const rt::Raytracer& raytracer) {
    return {
        raytracer.compileScene(create_scene_1()),
        raytracer.compileScene(create_scene_2()),
        raytracer.compileScene(create_scene_3()),
        raytracer.compileScene(create_scene_4()),
        raytracer.compileScene(create_scene_5()),
    };
}


int getCurrentSceneIndex() {
    static int currentSceneIndex = 4;

    if (IsKeyPressed(KEY_LEFT)) {
        currentSceneIndex--;
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        currentSceneIndex++;
    }

    return currentSceneIndex;
}


int main() {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raytracing with OpenCl");
    SetTargetFPS(30);

    rt::Raytracer raytracer(IMAGE_WIDTH, IMAGE_HEIGHT, rt::PixelFormat::R32G32B32A32, rt::RaytracingMode::MULTIPLE);
    rt::Renderer renderer(raytracer);
    rt::Camera camera = rt::create_camera(60.0f, {IMAGE_WIDTH, IMAGE_HEIGHT}, {0, 0, 6}, {0, 0, -1});

    std::vector<rt::CompiledScene> compiledScenes = initializeScenes(raytracer);
    int sceneIndex = getCurrentSceneIndex();
    rt::CompiledScene cScene = compiledScenes[sceneIndex % compiledScenes.size()];

    bool sceneChangedFlag = false;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (sceneIndex != getCurrentSceneIndex()) {
            raytracer.resetFrameCount();
            sceneIndex = getCurrentSceneIndex();
            cScene = compiledScenes[sceneIndex % compiledScenes.size()];
            sceneChangedFlag = true;
        }

        if (IsKeyPressed(KEY_SPACE) || sceneChangedFlag) {
            TIME_FUNCTION("renderScene", raytracer.renderScene(cScene, camera, rt::DEFAULT_CONFIG));
            TIME_FUNCTION("accumulatePixels", raytracer.accumulatePixels());
            renderer.update();
            sceneChangedFlag = false;
        }

        DrawTextureEx(renderer.get(), {0.0f, 0.0f}, 0.0f, (float) WINDOW_WIDTH/IMAGE_WIDTH, WHITE);

        // DrawFPS(10, 10);
        // DrawText(TextFormat("Frame time: %f", GetFrameTime()), 10, 30, 18, BLACK);
        EndDrawing();
    }

    CloseWindow();
}
