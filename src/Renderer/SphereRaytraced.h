#pragma once

#include "AK/Interval.h"
#include "Hittable.h"

class SphereRaytraced final : public Hittable
{
public:
    static std::shared_ptr<SphereRaytraced> create(float const radius, std::shared_ptr<Material> const& material);
    SphereRaytraced(AK::Badge<SphereRaytraced>, float const radius, std::shared_ptr<Material> const& material);

    virtual void initialize() override;
    virtual void update() override;
    virtual void draw() const override;

    virtual bool hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const override;

private:
    glm::vec3 m_center = {};
    float m_radius = 0.0f;
};
