
#pragma once

#include "src/raytracer/internal/camera.h"
#include <glm/gtc/matrix_transform.hpp>


namespace rt {

internal::Camera createCamera(float fov, const glm::ivec2& imageSize, const glm::vec3& position, const glm::vec3 direction) {
    glm::mat4 invViewMat = glm::inverse(glm::lookAt(
        position, position + direction, glm::vec3(0, 1, 0)
    ));
    glm::mat4 invProjMat = glm::inverse(glm::perspectiveFov(
        glm::radians(fov), (float) imageSize.x, (float) imageSize.y, 0.1f, 100.0f
    ));

    internal::Camera camera;
    memcpy(&camera.invViewMat, &invViewMat, sizeof(float) * 16);
    memcpy(&camera.invProjMat, &invProjMat, sizeof(float) * 16);
    camera.position = {position.x, position.y, position.z, 0.0f};
    camera.imageSize = {(uint32_t) imageSize.x, (uint32_t) imageSize.y};
    return camera;
}

}
