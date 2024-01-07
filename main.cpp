
#define RT_PRINT_LOG

#include "src/raylibrenderer.h"
#include "src/test_scenes.h"


#define WINDOW_WIDTH (1280)
#define WINDOW_HEIGHT (720)
#define IMAGE_SCALE (2.0f)
#define IMAGE_WIDTH (WINDOW_WIDTH / IMAGE_SCALE)
#define IMAGE_HEIGHT (WINDOW_HEIGHT / IMAGE_SCALE)


std::vector<rt::internal::Scene> createAllScenes(cl::Context context, cl::CommandQueue queue) {
    std::vector<rt::Scene> scenes = {
        createScene_1(),
        createScene_2(),
        createScene_3(),
        createScene_4(),
        createScene_5(),
    };
    std::vector<rt::internal::Scene> res;
    for (const auto& scene : scenes) {
        res.push_back(scene.convert(context, queue));
    }
    return res;
}


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


int main() {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raytracing with OpenCl");
    SetTargetFPS(30);

    cl::Platform platform = rt::getAllClPlatforms()[0];
    cl::Device device = rt::getAllClDevices(platform)[0];
    rt::CL_Objects clObjects = rt::createClObjects(platform, device);

    rt::Raytracer raytracer({IMAGE_WIDTH, IMAGE_HEIGHT}, clObjects, rt::Format::RGBA32F, true);
    rt::RaylibRenderer renderer(raytracer);

    auto camera = rt::createCamera(60.0f, {IMAGE_WIDTH, IMAGE_HEIGHT}, {0, 0, 6}, {0, 0, -1});
    auto scenes = createAllScenes(clObjects.context, clObjects.queue);
    int numScenes = scenes.size();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsKeyPressed(KEY_S) && IsKeyDown(KEY_LEFT_CONTROL)) {
            RT_LOG("Image save status: %s", raytracer.saveAsImage("result.png") ? "true" : "false");
        }

        if (isSceneChanged()) {
            raytracer.resetFrameCount();
        }

        if (IsKeyPressed(KEY_SPACE) || isSceneChanged()) {
            auto curScene = scenes[getSceneIndex(numScenes)];
            RT_TIME_STMT("renderScene", raytracer.renderScene(curScene, camera, {.sampleCount = 32, .bounceLimit = 5}));
            RT_TIME_STMT("accumulatePixels", raytracer.accumulatePixels());
            renderer.update();
        }

        renderer.draw(IMAGE_SCALE);

        DrawFPS(10, 10);
        DrawText(TextFormat("Frame time: %f", GetFrameTime()), 10, 30, 18, BLACK);
        EndDrawing();
    }
}
