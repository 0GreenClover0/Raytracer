#pragma once

#include "Hittable.h"

class SphereRaytraced final : public Hittable
{
public:
    static std::shared_ptr<SphereRaytraced> create(glm::vec3 const& center, float const radius, std::shared_ptr<Material> const& material);
    SphereRaytraced(AK::Badge<SphereRaytraced>, glm::vec3 const& center, float const radius, std::shared_ptr<Material> const& material);

    virtual void draw() const override;

    virtual bool hit(Ray const& ray, float const ray_tmin, float const ray_tmax, HitRecord& hit_record) const override;

private:
    glm::vec3 m_center;
    float m_radius;
};
