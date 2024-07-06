#include "Hittable.h"

#include "Raytracer.h"

Hittable::Hittable(std::shared_ptr<Material> const& material) : Drawable(material)
{
}

void Hittable::initialize()
{
    Drawable::initialize();

    Raytracer::get_instance()->register_hittable(static_pointer_cast<Hittable>(shared_from_this()));
}

void Hittable::uninitialize()
{
    Drawable::uninitialize();

    Raytracer::get_instance()->unregister_hittable(static_pointer_cast<Hittable>(shared_from_this()));
}

bool Hittable::hit_list(std::vector<std::weak_ptr<Hittable>> const& hittables, Ray const& ray, Interval const ray_t, HitRecord& hit_record)
{
    HitRecord temp_record;
    bool hit_anything = false;
    auto closest_so_far = ray_t.max;

    for (auto const& object : hittables)
    {
        if (object.lock()->hit(ray, Interval(ray_t.min, closest_so_far), temp_record))
        {
            hit_anything = true;
            closest_so_far = temp_record.t;
            hit_record = temp_record;
        }
    }

    return hit_anything;
}

AABB Hittable::bounding_box() const
{
    return m_bbox;
}
