
#include "src/backend/raylibrenderer.h"
#include "src/raytracer.h"


using namespace rt;


static Texture createRaylibTexture(uint32_t width, uint32_t height, Format format) {
    PixelFormat rlFormat;

    switch (format) {
        case Format::RGBA8:
            rlFormat = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
            break;
        case Format::RGBA32F:
            rlFormat = PIXELFORMAT_UNCOMPRESSED_R32G32B32A32;
            break;
        default:
            printf("ERROR (`createRaylibTexture`): No corresponding format available in raylib\n");
            break;
    }

    Image tempImage = GenImageColor(width, height, DARKBLUE);
    ImageFormat(&tempImage, rlFormat);
    Texture out = LoadTextureFromImage(tempImage);
    UnloadImage(tempImage);
    return out;
}


RaylibRenderer::RaylibRenderer(const Raytracer& raytracer, glm::ivec2 windowSize, int targetFps)
: m_raytracer(raytracer), m_windowSize(windowSize) {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(windowSize.x, windowSize.y, "Raytracing [with Raylib]");
    SetTargetFPS(targetFps);

    uint32_t bufferSize = m_raytracer.getPixelBufferSize();
    m_outBuffer = new uint8_t[bufferSize];
    const glm::ivec2& imgShape = m_raytracer.getImageShape();
    m_outTexture = createRaylibTexture(imgShape.x, imgShape.y, raytracer.getPixelFormat());
}


RaylibRenderer::~RaylibRenderer() {
    delete[] m_outBuffer;
    UnloadTexture(m_outTexture);
    CloseWindow();
}


void RaylibRenderer::update() {
    m_raytracer.readPixels(m_outBuffer);
    UpdateTexture(m_outTexture, m_outBuffer);
}


void RaylibRenderer::draw() {
    float scale = (float) m_windowSize.x / (float) m_outTexture.width;
    DrawTextureEx(m_outTexture, {0.0f, 0.0f}, 0.0f, scale, WHITE);
}
