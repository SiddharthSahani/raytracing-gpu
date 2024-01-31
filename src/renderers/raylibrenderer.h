
#pragma once

#include <raylib/raylib.h>
#include <glm/glm.hpp>


namespace rt {


class Raytracer;


class RaylibRenderer {

    public:
        RaylibRenderer(const Raytracer& raytracer, glm::ivec2 windowSize, int targetFps);
        ~RaylibRenderer();
        void update();
        void draw();

    private:
        const Raytracer& m_raytracer;
        glm::ivec2 m_windowSize;
        uint8_t* m_outBuffer;
        Texture m_outTexture;

};

}
