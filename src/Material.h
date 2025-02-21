#pragma once

#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "AK/Badge.h"
#include "AK/Types.h"
#include "Bounds.h"
#include "Shader.h"

class Drawable;
class TextureCPU;
class Ray;
struct HitRecord;

class Material
{
public:
    static std::shared_ptr<Material> create(std::shared_ptr<Shader> const& shader, i32 const render_order = 0,
                                            bool const is_gpu_instanced = false, bool const is_billboard = false,
                                            bool const is_transparent = false);

    static std::shared_ptr<Material> create(std::shared_ptr<Shader> const& shader, glm::vec3 const& color);

    explicit Material(AK::Badge<Material>, std::shared_ptr<Shader> const& shader, i32 const render_order, bool const is_gpu_instanced,
                      bool const is_billboard, bool is_transparent);

    [[nodiscard]] bool has_custom_render_order() const
    {
        return m_render_order != 0;
    }

    [[nodiscard]] i32 get_render_order() const;

    bool scatter(Ray const& ray_in, HitRecord const& hit_record, glm::vec3& attenuation, Ray& scattered) const;
    glm::vec3 emit(float const u, float const v, glm::vec3 const& point) const;

    std::shared_ptr<Shader> shader;

    // TODO: Expose properties directly from the shader, somehow.
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    std::shared_ptr<TextureCPU> texture = {};

    float specular = 1.0f;
    float shininess = 128.0f;

    bool metal = false;
    bool dielectric = false;
    bool emissive = false;
    bool isotropic = false;

    // Should be in 0.0 - 1.0 range
    float fuzz = 0.0f;

    float refraction_index = 0.0f;

    u32 sector_count = 5;
    u32 stack_count = 5;
    float radius_multiplier = 2.0f;

    // NOTE: This does not work for gpu instanced meshes, as model matrices are batched together in a single SSBO
    bool needs_view_model = false;

    bool needs_skybox = false;

    bool is_billboard = false;

    bool casts_shadows = true;

    bool needs_forward_rendering = false;

    bool is_transparent = false;

    // TODO: GPU instancing on one material currently supports only the first mesh that was bound to the material.
    bool is_gpu_instanced = false;

    // NOTE: Only valid if is_gpu_instanced is true
    std::vector<glm::mat4> model_matrices = {};
    std::vector<BoundingBoxShader> bounding_boxes = {};
    std::shared_ptr<Drawable> first_drawable = {};
    std::vector<std::shared_ptr<Drawable>> drawables = {};

private:
    static float reflectance(float cosine, float refraction_index);

    // TODO: Negative render order is currently not supported
    i32 m_render_order = 0;
};
