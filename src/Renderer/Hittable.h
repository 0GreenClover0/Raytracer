#pragma once

#include "Drawable.h"
#include "Game/Ray.h"

struct HitRecord
{
    glm::vec3 point;
    glm::vec3 normal;
    float t;
    bool front_face;

    void set_face_normal(Ray const& ray, glm::vec3 const& outward_normal)
    {
        // Sets the hit record normal vector.
        // NOTE: The parameter `outward_normal` is assumed to have unit length.

        front_face = glm::dot(ray.direction(), outward_normal) < 0.0f;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable : public Drawable
{
public:
    explicit Hittable(std::shared_ptr<Material> const& material);
    ~Hittable() override = default;

    virtual bool hit(Ray const& ray, float const ray_tmin, float const ray_tmax, HitRecord& hit_record) const = 0;
};
