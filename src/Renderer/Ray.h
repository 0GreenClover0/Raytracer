#pragma once

#include <glm/vec3.hpp>

class Ray
{
public:
    Ray() = default;

    Ray(glm::vec3 const& origin, glm::vec3 const& direction);

    [[nodiscard]] glm::vec3 const& origin() const;
    [[nodiscard]] glm::vec3 const& direction() const;

    [[nodiscard]] glm::vec3 at(float const t) const;

private:
    glm::vec3 m_origin = {};
    glm::vec3 m_direction = {};
};
