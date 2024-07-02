#pragma once

#include "AK/AABB.h"
#include "AK/Interval.h"
#include "Drawable.h"
#include "Ray.h"

struct HitRecord
{
    glm::vec3 point;
    glm::vec3 normal;
    std::shared_ptr<Material> material;
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

    virtual void initialize() override;
    virtual void uninitialize() override;

    virtual bool hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const = 0;

    AABB bounding_box() const;

protected:
    AABB m_bbox;
};
