#include "SphereRaytraced.h"

#include "glm/gtx/norm.hpp"

std::shared_ptr<SphereRaytraced> SphereRaytraced::create(glm::vec3 const& center, float const radius,
                                                         std::shared_ptr<Material> const& material)
{
    return std::make_shared<SphereRaytraced>(AK::Badge<SphereRaytraced> {}, center, radius, material);
}

SphereRaytraced::SphereRaytraced(AK::Badge<SphereRaytraced>, glm::vec3 const& center, float const radius,
                                 std::shared_ptr<Material> const& material)
    : Hittable(material), m_center(center), m_radius(radius)
{
}

void SphereRaytraced::draw() const
{
}

bool SphereRaytraced::hit(Ray const& ray, float const ray_tmin, float const ray_tmax, HitRecord& hit_record) const
{
    glm::vec3 const origin_center = m_center - ray.origin();
    float const a = glm::length2(ray.direction());
    float const h = glm::dot(ray.direction(), origin_center);
    float const c = glm::length2(origin_center) - m_radius * m_radius;

    float const discriminant = h * h - a * c;

    if (discriminant < 0.0f)
    {
        return false;
    }

    float const sqrt_discriminant = glm::sqrt(discriminant);

    float root = (h - sqrt_discriminant) / a;

    if (root <= ray_tmin || ray_tmax <= root)
    {
        root = (h + sqrt_discriminant) / a;

        if (root <= ray_tmin || ray_tmax <= root)
        {
            return false;
        }
    }

    hit_record.t = root;
    hit_record.point = ray.at(root);
    hit_record.normal = (hit_record.point - m_center) / m_radius;

    return true;
}
