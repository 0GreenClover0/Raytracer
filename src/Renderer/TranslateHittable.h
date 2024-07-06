#pragma once

#include "Renderer/Hittable.h"

class TranslateHittable final : public Hittable
{
public:
    TranslateHittable(std::shared_ptr<Hittable> const& hittable, glm::vec3 const& offset);

    virtual void initialize() override;

    virtual void draw() const override;

    virtual bool hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const override;

private:
    glm::vec3 m_offset = {};
    std::weak_ptr<Hittable> m_hittable = {};
};
