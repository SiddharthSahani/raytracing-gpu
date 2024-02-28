
#pragma once

#include "src/raytracer.h"
#include <glm/vec2.hpp>
namespace rl {
#include <raylib/raylib.h>
}


namespace rt {

rl::Texture createTexture(glm::ivec2 imageSize, Format format);


class Renderer {

    public:
        Renderer(const Raytracer& raytracer, glm::ivec2 windowSize, int targetFps, bool clglInterop);
        ~Renderer();
        void update();
        void draw();
        uint32_t getGlTexture() const { return m_outTexture.id; }

    private:
        const Raytracer& m_raytracer;
        glm::ivec2 m_windowSize;
        uint8_t* m_outBuffer;
        rl::Texture m_outTexture;
        bool m_clglInterop;

};

}
