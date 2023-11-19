
#pragma once

#include <CL/opencl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


namespace rt {

class Camera {

    public:
        Camera(float fov, const glm::vec2& viewport_size);
        void calculate_ray_directions(const glm::vec3& position, const glm::vec3& direction);
        const std::vector<cl_float3>& get_ray_directions() const { return m_ray_directions; }

    private:
        float m_fov;
        glm::vec2 m_viewport_size;
        std::vector<cl_float3> m_ray_directions;

};


Camera::Camera(float fov, const glm::vec2& viewport_size) {
    m_fov = fov;
    m_viewport_size = viewport_size;
    m_ray_directions.resize(viewport_size.x * viewport_size.y);
}


void Camera::calculate_ray_directions(const glm::vec3& position, const glm::vec3& direction) {
    glm::mat4 inv_view = glm::inverse(glm::lookAt(
        position, position + direction, glm::vec3(0, 1, 0)
    ));
    glm::mat4 inv_proj = glm::inverse(glm::perspectiveFov(
        glm::radians(m_fov), m_viewport_size.x, m_viewport_size.y, 0.1f, 100.0f
    ));

    for (uint32_t y = 0; y < m_viewport_size.y; y++) {
        for (uint32_t x = 0; x < m_viewport_size.x; x++) {
            glm::vec2 pixel_coord = {x, y};
            glm::vec2 coord = pixel_coord / m_viewport_size * 2.0f - 1.0f;

            glm::vec4 target = inv_proj * glm::vec4(coord.x, coord.y, 1.0f, 1.0f);
            glm::vec3 ray_direction = inv_view * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0.0f);
            m_ray_directions[x + (m_viewport_size.y-y-1) * m_viewport_size.x] = {ray_direction.x, ray_direction.y, ray_direction.z, 0.0f};
        }
    }
}

}
