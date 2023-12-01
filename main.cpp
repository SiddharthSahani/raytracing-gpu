
#define RT_LOG printf
#include "src/renderer.h"
#include "src/test_scenes.h"
#include <chrono>

using namespace std::chrono;


#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define IMAGE_WIDTH (1280/2)
#define IMAGE_HEIGHT (720/2)


#define TIME_FUNCTION(name, func) \
{\
    Timer timer(name);\
    func;\
}


// utility class to time functions
class Timer {

    public:
        Timer(const char* timerName) {
            m_timerName = timerName;
            m_startTime = high_resolution_clock::now();
        }
        ~Timer() {
            auto stopTime = high_resolution_clock::now();
            auto timeTaken = stopTime - m_startTime;
            auto timeTaken_ns = timeTaken.count();
            printf(
                "%s took %f secs (%f ms)\n",
                m_timerName,
                (float) timeTaken_ns / 1'000'000'000,
                (float) timeTaken_ns / 1'000'000
            );
        }

    private:
        const char* m_timerName;
        system_clock::time_point m_startTime;

};


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
            TIME_FUNCTION("renderScene", raytracer.renderScene(scene, camera, rt::DEFAULT_CONFIG));
            renderer.update();
        }

        DrawTextureEx(renderer.get(), {0.0f, 0.0f}, 0.0f, (float) WINDOW_WIDTH/IMAGE_WIDTH, WHITE);

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
}
