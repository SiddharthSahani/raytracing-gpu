
#pragma once

#include "src/raytracer.h"
#include <raylib/raylib.h>


static PixelFormat convertFormat(rt::PixelFormat format) {
    switch (format) {
        case rt::PixelFormat::R8G8B8A8:
            return PixelFormat::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        case rt::PixelFormat::R32G32B32A32:
            return PixelFormat::PIXELFORMAT_UNCOMPRESSED_R32G32B32A32;
        default:
            RT_LOG("Can not convert rt::PixelFormat %d using R32G32B32A32", (int) format);
            return PIXELFORMAT_UNCOMPRESSED_R32G32B32A32;
    }
}


namespace rt {

class Renderer {

    public:
        Renderer(const Raytracer& raytracer);
        ~Renderer();
        Texture get() const { return m_outTexture; }
        void update();

    private:
        const Raytracer* m_raytracer;
        uint8_t* m_outData;
        Texture m_outTexture;

};


Renderer::Renderer(const Raytracer& raytracer) {
    m_raytracer = &raytracer;

    uint32_t width = raytracer.getViewportWidth();
    uint32_t height = raytracer.getViewportHeight();
    uint32_t bufferSize = raytracer.getPixelBufferSize();
    m_outData = new uint8_t[bufferSize];

    Image tempImage = GenImageColor(width, height, BLACK);
    ImageFormat(&tempImage, convertFormat(raytracer.getPixelFormat()));
    m_outTexture = LoadTextureFromImage(tempImage);
    UnloadImage(tempImage);
}


Renderer::~Renderer() {
    delete[] m_outData;
    UnloadTexture(m_outTexture);
}


void Renderer::update() {
    m_raytracer->readPixels(m_outData);
    UpdateTexture(m_outTexture, m_outData);
}

}
