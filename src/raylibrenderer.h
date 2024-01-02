
#pragma once

#include "src/raytracer.cpp"
#include <raylib/raylib.h>


namespace rt {

static Texture createRaylibTexture(uint32_t width, uint32_t height, Format format) {
    Image tempImage = GenImageColor(width, height, BLACK);
    ImageFormat(&tempImage, format == Format::RGBA8 ? PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 : PIXELFORMAT_UNCOMPRESSED_R32G32B32A32);
    Texture texture = LoadTextureFromImage(tempImage);
    UnloadImage(tempImage);
    return texture;
}


class RaylibRenderer {

    public:
        RaylibRenderer(const Raytracer& raytracer);
        ~RaylibRenderer();
        void draw(float scale) const;
        void update();

    private:
        const Raytracer& m_raytracer;
        uint8_t* m_outBuffer;
        Texture m_outTexture;

};


RaylibRenderer::RaylibRenderer(const Raytracer& raytracer)
: m_raytracer(raytracer) {
    uint32_t bufferSize = m_raytracer.getPixelBufferSize();
    m_outBuffer = new uint8_t[bufferSize];

    glm::ivec2 shape = m_raytracer.getImageShape();
    uint32_t width = shape.x;
    uint32_t height = shape.y;
    m_outTexture = createRaylibTexture(width, height, raytracer.getPixelFormat());
}


RaylibRenderer::~RaylibRenderer() {
    delete[] m_outBuffer;
    UnloadTexture(m_outTexture);
}


void RaylibRenderer::draw(float scale) const {
    DrawTextureEx(m_outTexture, {0.0f, 0.0f}, 0.0f, scale, WHITE);
}


void RaylibRenderer::update() {
    m_raytracer.readPixels(m_outBuffer);
    UpdateTexture(m_outTexture, m_outBuffer);
}

}
