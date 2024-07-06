#pragma once

#include "Hittable.h"

class ConstantDensityMedium final : public Hittable
{
public:
    ConstantDensityMedium(std::vector<std::shared_ptr<Hittable>> const& boundary, float const density,
                          std::shared_ptr<Material> const& material, bool const disable_boundary_hits = true);

    virtual void initialize() override;

    virtual void draw() const override;

    virtual bool hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const override;

private:
    bool m_disable_boundary_hits = true;
    std::vector<std::weak_ptr<Hittable>> m_boundary;
    float m_negative_inverse_density = 0.0f;
};
