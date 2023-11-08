
#pragma once

#include "src/ray.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


namespace rt {

class Camera {

    public:
        Camera(float fov, const glm::vec2& viewport_size);
        void set_params(const glm::vec3& position, const glm::vec3& direction);
        const std::vector<Ray>& get_rays() const { return m_rays; }

    private:
        float m_fov;
        glm::vec2 m_viewport_size;
        glm::vec3 m_position;
        glm::vec3 m_direction;
        std::vector<Ray> m_rays;

};


Camera::Camera(float fov, const glm::vec2& viewport_size) {
    m_fov = fov;
    m_viewport_size = viewport_size;
    m_position = {0, 0, 6};
    m_direction = {0, 0, -1};

    m_rays.resize(viewport_size.x * viewport_size.y);
    set_params(m_position, m_direction);
}


void Camera::set_params(const glm::vec3& position, const glm::vec3& direction) {
    m_position = position;
    m_direction = direction;

    glm::mat4 view_mat = glm::lookAt(
        m_position, m_position + m_direction, glm::vec3(0, 1, 0)
    );
    glm::mat4 projection_mat = glm::perspectiveFov(
        glm::radians(m_fov), m_viewport_size.x, m_viewport_size.y, 0.1f, 100.0f
    );

    glm::mat4 inv_view = glm::inverse(view_mat);
    glm::mat4 inv_projection = glm::inverse(projection_mat);

    for (uint32_t y = 0; y < m_viewport_size.y; y++) {
        for (uint32_t x = 0; x < m_viewport_size.x; x++) {
            glm::vec2 coord = {x/m_viewport_size.x, y/m_viewport_size.y};
            coord = coord * 2.0f - 1.0f;

            glm::vec4 target = inv_projection * glm::vec4(coord.x, coord.y, 1.0f, 1.0f);
            glm::vec3 ray_dir = glm::vec3(inv_view * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0.0f));

            m_rays[x + y * m_viewport_size.x].origin = glm::vec4(m_position, 1.0f);
            m_rays[x + y * m_viewport_size.x].direction = glm::vec4(ray_dir, 1.0f);
        }
    }
}

}
