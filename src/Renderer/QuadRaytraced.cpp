#include "QuadRaytraced.h"

#include "Entity.h"
#include "Material.h"

std::shared_ptr<QuadRaytraced> QuadRaytraced::create(glm::vec3 const& q, glm::vec3 const& u, glm::vec3 const& v,
                                                     std::shared_ptr<Material> const& material)
{
    return std::make_shared<QuadRaytraced>(AK::Badge<QuadRaytraced> {}, q, u, v, material);
}

QuadRaytraced::QuadRaytraced(AK::Badge<QuadRaytraced>, glm::vec3 const& q, glm::vec3 const& u, glm::vec3 const& v,
                             std::shared_ptr<Material> material)
    : Hittable(material), m_q(q), m_u(u), m_v(v)
{
    glm::vec3 const n = glm::cross(u, v);
    m_normal = glm::normalize(n);
    m_d = glm::dot(m_normal, q);
    m_w = n / glm::dot(n, n);
}

void QuadRaytraced::initialize()
{
    Hittable::initialize();

    entity->transform->set_position(m_q);

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
    float const denominator = glm::dot(m_normal, ray.direction());

    // No hit if the ray is parallel to the plane.
    if (std::fabs(denominator) < 0.000001f)
    {
        return false;
    }

    // Return false if the hit point parameter t is outside the ray interval.
    float const t = (m_d - glm::dot(m_normal, ray.origin())) / denominator;

    if (!ray_t.contains(t))
    {
        return false;
    }

    // Determine the hit point lies within the planar shape using its plane coordinates.
    glm::vec3 const intersection = ray.at(t);

    glm::vec3 const planar_hit = intersection - m_q;

    float const alpha = glm::dot(m_w, glm::cross(planar_hit, m_v));
    float const beta = glm::dot(m_w, glm::cross(m_u, planar_hit));

    if (!is_interior(alpha, beta, hit_record))
    {
        return false;
    }

    // Ray hits the 2D shape; set the rest of the hit record and return true.
    hit_record.t = t;
    hit_record.point = intersection;
    hit_record.material = material;
    hit_record.set_face_normal(ray, m_normal);

    return true;
}

bool QuadRaytraced::is_interior(float const a, float const b, HitRecord& hit_record)
{
    static Interval const unit_interval(0.0f, 1.0f);

    // Given the hit point in plane coordinates, return false if it is outside the
    // primitive, otherwise set the hit record UV coordinates and return true.
    if (!unit_interval.contains(a) || !unit_interval.contains(b))
    {
        return false;
    }

    hit_record.u = a;
    hit_record.v = b;

    return true;
}

void QuadRaytraced::set_bounding_box()
{
    auto const bbox_diagonal1 = AABB(m_q, m_q + m_u + m_v);
    auto const bbox_diagonal2 = AABB(m_q + m_u, m_q + m_v);
    m_bbox = AABB(bbox_diagonal1, bbox_diagonal2);
}

void QuadRaytraced::box(glm::vec3 const& a, glm::vec3 const& b, std::shared_ptr<Material> const& material)
{
    // Construct the two opposite vertices with the minimum and maximum coordinates.
    auto const min = glm::vec3(std::fmin(a.x, b.x), std::fmin(a.y, b.y), std::fmin(a.z, b.z));
    auto const max = glm::vec3(std::fmax(a.x, b.x), std::fmax(a.y, b.y), std::fmax(a.z, b.z));

    auto const delta_x = glm::vec3(max.x - min.x, 0.0f, 0.0f);
    auto const delta_y = glm::vec3(0.0f, max.y - min.y, 0.0f);
    auto const delta_z = glm::vec3(0.0f, 0.0f, max.z - min.z);

    auto entity = Entity::create("BoxSide");
    entity->add_component<QuadRaytraced>(create(glm::vec3(min.x, min.y, max.z), delta_x, delta_y, material)); // Front

    entity = Entity::create("BoxSide");
    entity->add_component<QuadRaytraced>(create(glm::vec3(max.x, min.y, max.z), -delta_z, delta_y, material)); // Right

    entity = Entity::create("BoxSide");
    entity->add_component<QuadRaytraced>(create(glm::vec3(max.x, min.y, min.z), -delta_x, delta_y, material)); // Back

    entity = Entity::create("BoxSide");
    entity->add_component<QuadRaytraced>(create(glm::vec3(min.x, min.y, min.z), delta_z, delta_y, material)); // Left

    entity = Entity::create("BoxSide");
    entity->add_component<QuadRaytraced>(create(glm::vec3(min.x, max.y, max.z), delta_x, -delta_z, material)); // Top

    entity = Entity::create("BoxSide");
    entity->add_component<QuadRaytraced>(create(glm::vec3(min.x, min.y, min.z), delta_x, delta_z, material)); // Bottom
}
