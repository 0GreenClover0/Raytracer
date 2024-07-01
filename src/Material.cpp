#include "Material.h"

#include "Renderer.h"
#include "Renderer/Hittable.h"
#include "Renderer/Ray.h"

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
        glm::vec3 const reflected = glm::reflect(ray_in.direction(), hit_record.normal);
        scattered = Ray(hit_record.point, reflected);
        attenuation = color;
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
        attenuation = color;
        return true;
    }
}
