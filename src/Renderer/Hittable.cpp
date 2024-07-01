#include "Hittable.h"

Hittable::Hittable(std::shared_ptr<Material> const& material) : Drawable(material)
{
}
