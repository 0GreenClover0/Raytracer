#include "Ray.h"

Ray::Ray(glm::vec3 const& origin, glm::vec3 const& direction) : m_origin(origin), m_direction(direction)
{
}

glm::vec3 const& Ray::origin() const
{
    return m_origin;
}

glm::vec3 const& Ray::direction() const
{
    return m_direction;
}

glm::vec3 Ray::at(float const t) const
{
    return m_origin + t * m_direction;
}
