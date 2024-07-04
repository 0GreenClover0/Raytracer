#pragma once

#include "AK/Badge.h"
#include "Hittable.h"

class QuadRaytraced final : public Hittable
{
public:
    static std::shared_ptr<QuadRaytraced> create(glm::vec3 const& q, glm::vec3 const& u, glm::vec3 const& v,
                                                 std::shared_ptr<Material> const& material);

    QuadRaytraced(AK::Badge<QuadRaytraced>, glm::vec3 const& q, glm::vec3 const& u, glm::vec3 const& v, std::shared_ptr<Material> material);

    virtual void initialize() override;
    virtual void update() override;
    virtual void draw() const override;

    virtual bool hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const override;

private:
    void set_bounding_box();

    glm::vec3 m_q = {};
    glm::vec3 m_u = {};
    glm::vec3 m_v = {};
};
