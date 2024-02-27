
#include "src/backend/raylibcamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <raylib/raylib.h>


namespace rt {

Camera::Camera(float fov, const glm::ivec2& imageSize, const glm::vec3& position, const glm::vec3& direction, const CameraParams& params)
: m_position(position), m_direction(direction), m_params(params) {

    glm::mat4 invProjMat = glm::inverse(glm::perspectiveFov(
        glm::radians(fov), (float) imageSize.x, (float) imageSize.y, 0.1f, 100.0f
    ));
    glm::mat4 invViewMat = glm::inverse(glm::lookAt(
        position, position + direction, glm::vec3(0, 1, 0)
    ));

    memcpy(&m_internal.invProjMat, &invProjMat, sizeof(float) * 16);
    memcpy(&m_internal.invViewMat, &invViewMat, sizeof(float) * 16);
    m_internal.imageSize = {(uint32_t) imageSize.x, (uint32_t) imageSize.y};
    m_internal.position = {position.x, position.y, position.z, 0.0f};
}


bool Camera::update(float timestep) {
    auto _delta = GetMouseDelta();
    glm::vec2 delta = glm::vec2(_delta.x, _delta.y) * m_params.sensitivity;

    static bool rmbPressed = false;

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && !rmbPressed) {
        rmbPressed = true;
        DisableCursor();
    }
    if (!IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        if (rmbPressed) {
            rmbPressed = false;
            EnableCursor();
        }
        return false;
    }

    bool moved = false;

    glm::vec3 upDirection = {0, 1, 0};
    glm::vec3 rightDirection = glm::cross(m_direction, upDirection);

    if (IsKeyDown(KEY_W)) {
        m_position += m_direction * m_params.speed * timestep;
        moved = true;
    }
    if (IsKeyDown(KEY_S)) {
        m_position -= m_direction * m_params.speed * timestep;
        moved = true;
    }
    if (IsKeyDown(KEY_A)) {
        m_position -= rightDirection * m_params.speed * timestep;
        moved = true;
    }
    if (IsKeyDown(KEY_D)) {
        m_position += rightDirection * m_params.speed * timestep;
        moved = true;
    }
    if (IsKeyDown(KEY_Q)) {
        m_position += upDirection * m_params.speed * timestep;
        moved = true;
    }
    if (IsKeyDown(KEY_E)) {
        m_position -= upDirection * m_params.speed * timestep;
        moved = true;
    }

    // mouse movement
    if (delta.x != 0.0f || delta.y != 0.0f) {
        float yaw_delta = delta.x * m_params.rotationSpeed;
        float pitch_delta = delta.y * m_params.rotationSpeed;

        glm::quat q = glm::normalize(glm::cross(
            glm::angleAxis(-pitch_delta, rightDirection),
            glm::angleAxis(-yaw_delta, upDirection)
        ));

        m_direction = glm::rotate(q, m_direction);

        moved = true;
    }

    if (moved) {
        glm::mat4 invViewMat = glm::inverse(glm::lookAt(
            m_position, m_position + m_direction, upDirection
        ));
        memcpy(&m_internal.invViewMat, &invViewMat, sizeof(float) * 16);
        m_internal.position = {m_position.x, m_position.y, m_position.z, 0.0f};
    }

    return moved;
}

}
