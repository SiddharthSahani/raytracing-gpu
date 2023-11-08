
#include <raylib/raylib.h>


int main() {
    InitWindow(1280, 720, "OpenCl with raylib");
    SetTargetFPS(30);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
}
