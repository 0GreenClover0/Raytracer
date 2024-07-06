#include "ConstantDensityMedium.h"

#include "AK/AK.h"
#include "AK/Math.h"
#include "Raytracer.h"

ConstantDensityMedium::ConstantDensityMedium(std::vector<std::shared_ptr<Hittable>> const& boundary, float const density,
                                             std::shared_ptr<Material> const& material, bool const disable_boundary_hits)
    : Hittable(material), m_disable_boundary_hits(disable_boundary_hits), m_negative_inverse_density(-1.0f / density)
{
    for (auto const& hittable : boundary)
    {
        m_boundary.emplace_back(hittable);
    }
}

void ConstantDensityMedium::initialize()
{
    Hittable::initialize();

    for (auto const& hittable : m_boundary)
    {
        if (m_disable_boundary_hits)
        {
            Raytracer::get_instance()->unregister_hittable(hittable.lock());
        }

        m_bbox = AABB(m_bbox, hittable.lock()->bounding_box());
    }
}

void ConstantDensityMedium::draw() const
{
}

bool ConstantDensityMedium::hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const
{
    HitRecord record1;
    HitRecord record2;

    if (!hit_list(m_boundary, ray, Interval::whole, record1))
    {
        return false;
    }

    if (!hit_list(m_boundary, ray, Interval(record1.t + 0.0001f, AK::INFINITY_F), record2))
    {
        return false;
    }

    if (record1.t < ray_t.min)
    {
        record1.t = ray_t.min;
    }

    if (record2.t > ray_t.max)
    {
        record2.t = ray_t.max;
    }

    if (record1.t >= record2.t)
    {
        return false;
    }

    if (record1.t < 0.0f)
    {
        record1.t = 0.0f;
    }

    float const ray_length = glm::length(ray.direction());
    float const distance_inside_boundary = (record2.t - record1.t) * ray_length;
    float const hit_distance = m_negative_inverse_density * std::log(AK::random_float_fast());

    if (hit_distance > distance_inside_boundary)
    {
        return false;
    }

    hit_record.t = record1.t + hit_distance / ray_length;
    hit_record.point = ray.at(hit_record.t);
    hit_record.normal = glm::vec3(1.0f, 0.0f, 0.0f); // Arbitrary
    hit_record.front_face = true; // Arbitrary
    hit_record.material = material;

    return true;
}
