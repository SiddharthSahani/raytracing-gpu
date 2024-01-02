
#define RT_PRINT_LOG

#include "src/raylibrenderer.h"
#include "src/test_scenes.h"


#define WINDOW_WIDTH (1280)
#define WINDOW_HEIGHT (720)
#define IMAGE_SCALE (2.0f)
#define IMAGE_WIDTH (WINDOW_WIDTH / IMAGE_SCALE)
#define IMAGE_HEIGHT (WINDOW_HEIGHT / IMAGE_SCALE)


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
    auto scene = create_scene_5().convert(clObjects.context, clObjects.queue);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsKeyPressed(KEY_SPACE)) {
            RT_TIME_STMT("renderScene", raytracer.renderScene(scene, camera, {.sampleCount = 32, .bounceLimit = 5}));
            RT_TIME_STMT("accumulatePixels", raytracer.accumulatePixels());
            renderer.update();
        }

        renderer.draw(IMAGE_SCALE);

        DrawFPS(10, 10);
        DrawText(TextFormat("Frame time: %f", GetFrameTime()), 10, 30, 18, BLACK);
        EndDrawing();
    }
}
