
#pragma once

#include "src/raytracer/internal/camera.h"
#include <glm/glm.hpp>


namespace rt {

struct CameraParams {
    float sensitivity = 0.002f;
    float speed = 5.0f;
    float rotationSpeed = 2.0f;
};


class Camera {

    public:
        Camera(float fov, const glm::ivec2& imageSize, const glm::vec3& position, const glm::vec3& direction, const CameraParams& params);
        bool update(float timestep);
        const internal::Camera& getInternal() const { return m_internal; }

    private:
        glm::vec3 m_position;
        glm::vec3 m_direction;
        internal::Camera m_internal;
        CameraParams m_params;

};

}
