#include "SphereRaytraced.h"

#include "Entity.h"

#include <glm/gtx/norm.hpp>

std::shared_ptr<SphereRaytraced> SphereRaytraced::create(float const radius, std::shared_ptr<Material> const& material)
{
    return std::make_shared<SphereRaytraced>(AK::Badge<SphereRaytraced> {}, radius, material);
}

SphereRaytraced::SphereRaytraced(AK::Badge<SphereRaytraced>, float const radius, std::shared_ptr<Material> const& material)
    : Hittable(material), m_radius(radius)
{
}

void SphereRaytraced::initialize()
{
    Hittable::initialize();

    m_center = entity->transform->get_position();

    glm::vec3 const radius_vec = {m_radius, m_radius, m_radius};
    m_bbox = AABB(m_center - radius_vec, m_center + radius_vec);
}

void SphereRaytraced::update()
{
    m_center = entity->transform->get_position();
}

void SphereRaytraced::draw() const
{
}

bool SphereRaytraced::hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const
{
    glm::vec3 const origin_center = m_center - ray.origin();
    glm::vec3 const direction = ray.direction();
    float const a = glm::length2(direction);
    float const h = glm::dot(direction, origin_center);
    float const c = glm::length2(origin_center) - m_radius * m_radius;

    float const discriminant = h * h - a * c;

    if (discriminant < 0.0f)
    {
        return false;
    }

    float const sqrt_discriminant = glm::sqrt(discriminant);

    float root = (h - sqrt_discriminant) / a;

    if (root <= ray_t.min || ray_t.max <= root)
    {
        root = (h + sqrt_discriminant) / a;

        if (root <= ray_t.min || ray_t.max <= root)
        {
            return false;
        }
    }

    hit_record.t = root;
    hit_record.point = ray.at(root);
    glm::vec3 const outward_normal = (hit_record.point - m_center) / m_radius;
    hit_record.material = material;
    get_sphere_uv(outward_normal, hit_record.u, hit_record.v);
    hit_record.set_face_normal(ray, outward_normal);

    return true;
}

void SphereRaytraced::get_sphere_uv(glm::vec3 const& point, float& u, float& v)
{
    // p: a given point on the sphere of radius one, centered at the origin.
    // u: returned value [0,1] of angle around the Y axis from X=-1.
    // v: returned value [0,1] of angle from Y=-1 to Y=+1.
    //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

    float const theta = std::acos(-point.y);
    float const phi = std::atan2(-point.z, point.x) + glm::pi<float>();

    u = phi / (2.0f * glm::pi<float>());
    v = theta / glm::pi<float>();
}
