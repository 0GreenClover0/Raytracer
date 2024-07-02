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

AABB Hittable::bounding_box() const
{
    return m_bbox;
}
