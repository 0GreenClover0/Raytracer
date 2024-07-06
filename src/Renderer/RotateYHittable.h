#pragma once

#include "Hittable.h"

class RotateYHittable final : public Hittable
{
public:
    RotateYHittable(std::shared_ptr<Hittable> const& hittable, float const angle);

    virtual void initialize() override;

    virtual void draw() const override;

    virtual bool hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const override;

private:
    std::weak_ptr<Hittable> m_hittable;
    float m_sin_theta = 0.0f;
    float m_cos_theta = 0.0f;
};
