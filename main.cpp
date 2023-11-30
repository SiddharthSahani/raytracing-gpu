
#define RT_LOG printf
#include "src/renderer.h"
#include "src/test_scenes.h"


#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define IMAGE_WIDTH (1280/2)
#define IMAGE_HEIGHT (720/2)


int main() {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Raytracing with OpenCl");
    SetTargetFPS(30);

    rt::Raytracer raytracer(rt::PixelFormat::R32G32B32A32, IMAGE_WIDTH, IMAGE_HEIGHT);
    rt::Renderer renderer(raytracer);
    rt::Camera camera = rt::create_camera(60.0f, {IMAGE_WIDTH, IMAGE_HEIGHT}, {0, 0, 6}, {0, 0, -1});
    rt::CompiledScene scene = create_scene_1();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsKeyPressed(KEY_SPACE)) {
            raytracer.renderScene(scene, camera, rt::DEFAULT_CONFIG);
            renderer.update();
        }

        DrawTextureEx(renderer.get(), {0.0f, 0.0f}, 0.0f, (float) WINDOW_WIDTH/IMAGE_WIDTH, WHITE);

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
}
