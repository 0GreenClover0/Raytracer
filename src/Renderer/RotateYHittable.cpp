#include "RotateYHittable.h"

#include "AK/Math.h"
#include "Raytracer.h"

RotateYHittable::RotateYHittable(std::shared_ptr<Hittable> const& hittable, float const angle)
    : Hittable(hittable->material), m_hittable(hittable)
{
    float const radians = glm::radians(angle);
    m_sin_theta = glm::sin(radians);
    m_cos_theta = glm::cos(radians);

    m_bbox = hittable->bounding_box();

    glm::vec3 min(AK::INFINITY_F, AK::INFINITY_F, AK::INFINITY_F);
    glm::vec3 max(-AK::INFINITY_F, -AK::INFINITY_F, -AK::INFINITY_F);

    for (i32 i = 0; i < 2; ++i)
    {
        for (i32 k = 0; k < 2; ++k)
        {
            for (i32 m = 0; m < 2; ++m)
            {
                float const x = static_cast<float>(i) * m_bbox.x.max + static_cast<float>(1 - i) * m_bbox.x.min;
                float const y = static_cast<float>(k) * m_bbox.y.max + static_cast<float>(1 - k) * m_bbox.y.min;
                float const z = static_cast<float>(m) * m_bbox.z.max + static_cast<float>(1 - m) * m_bbox.z.min;

                float const new_x = m_cos_theta * x + m_sin_theta * z;
                float const new_z = -m_sin_theta * x + m_cos_theta * z;

                glm::vec3 tester(new_x, y, new_z);

                for (i32 c = 0; c < 3; ++c)
                {
                    min[c] = std::fmin(min[c], tester[c]);
                    max[c] = std::fmax(max[c], tester[c]);
                }
            }
        }
    }

    m_bbox = AABB(min, max);
}

void RotateYHittable::initialize()
{
    Hittable::initialize();

    Raytracer::get_instance()->unregister_hittable(m_hittable.lock());
}

void RotateYHittable::draw() const
{
}

bool RotateYHittable::hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const
{
    // Change the ray from world space to object space.
    glm::vec3 origin = ray.origin();
    glm::vec3 direction = ray.direction();

    origin.x = m_cos_theta * ray.origin().x - m_sin_theta * ray.origin().z;
    origin.z = m_sin_theta * ray.origin().x + m_cos_theta * ray.origin().z;

    direction.x = m_cos_theta * ray.direction().x - m_sin_theta * ray.direction().z;
    direction.z = m_sin_theta * ray.direction().x + m_cos_theta * ray.direction().z;

    Ray const rotated_ray(origin, direction);

    // Determine whether an intersection exists in object space (and if so, where).
    if (m_hittable.expired() || !m_hittable.lock()->hit(rotated_ray, ray_t, hit_record))
        return false;

    // Change the intersection point from object space to world space.
    glm::vec3 point = hit_record.point;
    point.x = m_cos_theta * hit_record.point.x + m_sin_theta * hit_record.point.z;
    point.z = -m_sin_theta * hit_record.point.x + m_cos_theta * hit_record.point.z;

    // Change the normal from object space to world space.
    glm::vec3 normal = hit_record.normal;
    normal.x = m_cos_theta * hit_record.normal.x + m_sin_theta * hit_record.normal.z;
    normal.z = -m_sin_theta * hit_record.normal.x + m_cos_theta * hit_record.normal.z;

    hit_record.point = point;
    hit_record.normal = normal;

    return true;
}
