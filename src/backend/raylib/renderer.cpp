
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

    rl::Image temp = rl::GenImageGradientRadial(imageSize.x, imageSize.y, 0.7f, rl::RED, rl::BLUE);
    rl::ImageFormat(&temp, rlFormat);
    rl::Texture out = rl::LoadTextureFromImage(temp);
    rl::UnloadImage(temp);
    return out;
}


Renderer::Renderer(const Raytracer& raytracer, glm::ivec2 windowSize, int targetFps, rl::Texture outTexture, bool clglInterop)
: m_raytracer(raytracer), m_windowSize(windowSize), m_outTexture(outTexture), m_clglInterop(clglInterop) {
    rl::SetTargetFPS(targetFps);

    if (m_clglInterop) {
        m_outBuffer = nullptr;
    } else {
        uint32_t bufferSize = m_raytracer.getPixelBufferSize();
        m_outBuffer = new uint8_t[bufferSize];
    }
}


Renderer::~Renderer() {
    delete[] m_outBuffer;
    rl::UnloadTexture(m_outTexture);
    rl::CloseWindow();
}


void Renderer::update() {
    if (!m_clglInterop) {
        m_raytracer.readPixels(m_outBuffer);
        rl::UpdateTexture(m_outTexture, m_outBuffer);
    }
}


void Renderer::draw() {
    float scale = (float) m_windowSize.x / (float) m_outTexture.width;
    rl::DrawTextureEx(m_outTexture, {0.0f, 0.0f}, 0.0f, scale, rl::WHITE);
}

}
