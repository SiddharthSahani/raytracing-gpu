
#pragma once

#include <CL/opencl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


namespace rt {


struct clCamera {
    cl_float16 inv_view;
    cl_float16 inv_proj;
    cl_float3 position;
    cl_uint2 image_size;
};


clCamera create_camera(float fov, const glm::ivec2& image_size, const glm::vec3& position, const glm::vec3 direction) {
    glm::mat4 inv_view = glm::inverse(glm::lookAt(
        position, position + direction, glm::vec3(0, 1, 0)
    ));
    glm::mat4 inv_proj = glm::inverse(glm::perspectiveFov(
        glm::radians(fov), (float) image_size.x, (float) image_size.y, 0.1f, 100.0f
    ));

    clCamera camera;
    memcpy(&camera.inv_view, &inv_view, sizeof(float) * 16);
    memcpy(&camera.inv_proj, &inv_proj, sizeof(float) * 16);
    camera.position = {position.x, position.y, position.z, 0.0f};
    camera.image_size = {(uint32_t) image_size.x, (uint32_t) image_size.y};
    return camera;
}

}
