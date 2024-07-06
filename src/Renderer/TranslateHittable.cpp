#include "TranslateHittable.h"

#include "Raytracer.h"

TranslateHittable::TranslateHittable(std::shared_ptr<Hittable> const& hittable, glm::vec3 const& offset)
    : Hittable(hittable->material), m_offset(offset), m_hittable(hittable)
{
}

void TranslateHittable::initialize()
{
    Hittable::initialize();

    m_bbox = m_hittable.lock()->bounding_box() + m_offset;

    Raytracer::get_instance()->unregister_hittable(m_hittable.lock());
}

void TranslateHittable::draw() const
{
}

bool TranslateHittable::hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const
{
    // Move the ray backwards by the offset
    Ray const offset_ray(ray.origin() - m_offset, ray.direction());

    // Determine whether an intersection exists along the offset ray (and if so, where)
    if (m_hittable.expired() || !m_hittable.lock()->hit(offset_ray, ray_t, hit_record))
        return false;

    // Move the intersection point forwards by the offset
    hit_record.point += m_offset;

    return true;
}
