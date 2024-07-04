#include "QuadRaytraced.h"

#include "Entity.h"

std::shared_ptr<QuadRaytraced> QuadRaytraced::create(glm::vec3 const& q, glm::vec3 const& u, glm::vec3 const& v,
                                                     std::shared_ptr<Material> const& material)
{
    return std::make_shared<QuadRaytraced>(AK::Badge<QuadRaytraced> {}, q, u, v, material);
}

QuadRaytraced::QuadRaytraced(AK::Badge<QuadRaytraced>, glm::vec3 const& q, glm::vec3 const& u, glm::vec3 const& v,
                             std::shared_ptr<Material> material)
    : Hittable(material), m_q(q), m_u(u), m_v(v)
{
}

void QuadRaytraced::initialize()
{
    Hittable::initialize();

    m_q = entity->transform->get_position();

    set_bounding_box();
}

void QuadRaytraced::update()
{
    // FIXME: This is actually not called because can_tick is set to false. When we'll set it to true eventually,
    // AABB will need to be updated as well.
    m_q = entity->transform->get_position();
}

void QuadRaytraced::draw() const
{
}

bool QuadRaytraced::hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const
{
    return false; // TODO: Implement
}

void QuadRaytraced::set_bounding_box()
{
    auto const bbox_diagonal1 = AABB(m_q, m_q + m_u + m_v);
    auto const bbox_diagonal2 = AABB(m_q + m_u, m_q + m_v);
    m_bbox = AABB(bbox_diagonal1, bbox_diagonal2);
}
