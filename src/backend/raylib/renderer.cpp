
#include "src/backend/raylib/renderer.h"
#include "src/raytracer.h"


namespace rt {

rl::Texture createTexture(glm::ivec2 imageSize, Format format) {
    rl::PixelFormat rlFormat;
    switch (format) {
        case Format::RGBA8:
            rlFormat = rl::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
            break;
        case Format::RGBA32F:
            rlFormat = rl::PIXELFORMAT_UNCOMPRESSED_R32G32B32A32;
            break;
        case Format::RGBA16F:
            rlFormat = rl::PIXELFORMAT_UNCOMPRESSED_R16G16B16A16;
            break;
        default:
            printf("ERROR (`createTexture`): Not implemented for Format::%d\n", format);
    }

    rl::Image temp = rl::GenImageGradientRadial(imageSize.x, imageSize.y, 0.3f, rl::RED, rl::BLUE);
    rl::ImageFormat(&temp, rlFormat);
    rl::Texture out = rl::LoadTextureFromImage(temp);
    rl::UnloadImage(temp);
    return out;
}


Renderer::Renderer(const Raytracer& raytracer, glm::ivec2 windowSize, int targetFps)
: m_raytracer(raytracer), m_windowSize(windowSize) {
    rl::SetTargetFPS(targetFps);

    uint32_t bufferSize = m_raytracer.getPixelBufferSize();
    m_outBuffer = new uint8_t[bufferSize];

    glm::ivec2 imageSize = raytracer.getImageShape();
    m_outTexture = createTexture(imageSize, raytracer.getPixelFormat());
}


Renderer::~Renderer() {
    delete[] m_outBuffer;
    rl::UnloadTexture(m_outTexture);
    rl::CloseWindow();
}


void Renderer::update() {
    m_raytracer.readPixels(m_outBuffer);
    rl::UpdateTexture(m_outTexture, m_outBuffer);
}


void Renderer::draw() {
    float scale = (float) m_windowSize.x / (float) m_outTexture.width;
    rl::DrawTextureEx(m_outTexture, {0.0f, 0.0f}, 0.0f, scale, rl::WHITE);
}

}
