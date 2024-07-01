#pragma once

#include "Drawable.h"
#include "Game/Ray.h"

struct HitRecord
{
    glm::vec3 point;
    glm::vec3 normal;
    float t;
};

class Hittable : public Drawable
{
public:
    explicit Hittable(std::shared_ptr<Material> const& material);
    ~Hittable() override = default;

    virtual bool hit(Ray const& ray, float const ray_tmin, float const ray_tmax, HitRecord& hit_record) const = 0;
};
