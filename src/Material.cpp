#include "Material.h"

#include "AK/AK.h"
#include "AK/Math.h"
#include "Renderer.h"
#include "Renderer/Hittable.h"
#include "Renderer/Ray.h"
#include "Renderer/TextureCPU.h"

std::shared_ptr<Material> Material::create(std::shared_ptr<Shader> const& shader, i32 const render_order, bool const is_gpu_instanced,
                                           bool const is_billboard, bool const is_transparent)
{
    auto material =
        std::make_shared<Material>(AK::Badge<Material> {}, shader, render_order, is_gpu_instanced, is_billboard, is_transparent);

    if (is_transparent)
    {
        material->m_render_order = Renderer::transparent_render_order;
    }

    if (render_order > 0)
    {
        material->needs_forward_rendering = true;
    }

    return material;
}

std::shared_ptr<Material> Material::create(std::shared_ptr<Shader> const& shader, glm::vec3 const& color)
{
    auto material = std::make_shared<Material>(AK::Badge<Material> {}, shader, 0, false, false, false);

    material->texture = std::make_shared<SolidColor>(color);

    return material;
}

Material::Material(AK::Badge<Material>, std::shared_ptr<Shader> const& shader, i32 const render_order, bool const is_gpu_instanced,
                   bool const is_billboard, bool const is_transparent)
    : shader(shader), is_billboard(is_billboard), is_transparent(is_transparent), is_gpu_instanced(is_gpu_instanced),
      m_render_order(render_order)
{
}

i32 Material::get_render_order() const
{
    return m_render_order;
}

bool Material::scatter(Ray const& ray_in, HitRecord const& hit_record, glm::vec3& attenuation, Ray& scattered) const
{
    if (metal)
    {
        glm::vec3 reflected = glm::reflect(ray_in.direction(), hit_record.normal);
        reflected = glm::normalize(reflected) + (fuzz * AK::Math::random_unit_vector());
        scattered = Ray(hit_record.point, reflected);
        attenuation = color;
        return glm::dot(scattered.direction(), hit_record.normal) > 0.0f;
    }
    else if (dielectric)
    {
        attenuation = glm::vec3(1.0f, 1.0f, 1.0f);
        float const ri = hit_record.front_face ? (1.0f / refraction_index) : refraction_index;

        glm::vec3 const unit_direction = glm::normalize(ray_in.direction());

        float const cos_theta = glm::min(glm::dot(-unit_direction, hit_record.normal), 1.0f);
        float const sin_theta = glm::sqrt(1.0f - cos_theta * cos_theta);

        bool const cannot_refract = ri * sin_theta > 1.0f;
        glm::vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > AK::random_float_fast())
        {
            direction = glm::reflect(unit_direction, hit_record.normal);
        }
        else
        {
            direction = glm::refract(unit_direction, hit_record.normal, ri);
        }

        scattered = Ray(hit_record.point, direction);
        return true;
    }
    else
    {
        glm::vec3 scatter_direction = hit_record.normal + AK::Math::random_unit_vector();

        if (AK::Math::are_nearly_equal(scatter_direction, glm::vec3(0.0f, 0.0f, 0.0f)))
        {
            scatter_direction = hit_record.normal;
        }

        scattered = Ray(hit_record.point, scatter_direction);

        if (texture == nullptr)
        {
            attenuation = color;
        }
        else
        {
            attenuation = texture->value(hit_record.u, hit_record.v, hit_record.point);
        }

        return true;
    }
}

float Material::reflectance(float cosine, float refraction_index)
{
    // Use Schlick's approximation for reflectance.
    float r0 = (1.0f - refraction_index) / (1.0f + refraction_index);
    r0 = r0 * r0;
    return r0 + (1.0f - r0) * glm::pow((1.0f - cosine), 5.0f);
}
