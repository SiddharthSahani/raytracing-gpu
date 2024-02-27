
#pragma once

#include "src/raytracer/internal/camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace rt {

static internal::Camera createCamera(float fov, const glm::ivec2& imageSize, const glm::vec3& position,const glm::vec3& direction) {
    internal::Camera out;

    glm::mat4 invProjMat = glm::inverse(glm::perspectiveFov(
        glm::radians(fov), (float) imageSize.x, (float) imageSize.y, 0.1f, 100.0f
    ));
    glm::mat4 invViewMat = glm::inverse(glm::lookAt(
        position, position + direction, glm::vec3(0, 1, 0)
    ));

    memcpy(&out.invProjMat, &invProjMat, sizeof(float) * 16);
    memcpy(&out.invViewMat, &invViewMat, sizeof(float) * 16);
    out.imageSize = {(uint32_t) imageSize.x, (uint32_t) imageSize.y};
    out.position = {position.x, position.y, position.z, 0.0f};

    return out;
}

}
