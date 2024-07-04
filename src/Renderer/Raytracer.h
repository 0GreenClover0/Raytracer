#pragma once

#include "AK/Badge.h"
#include "AK/Interval.h"
#include "Ray.h"
#include "Renderer/Hittable.h"

#include <glm/vec3.hpp>

#include <memory>
#include <string>
#include <vector>

class BVHNode;
class Hittable;
class Camera;

class Raytracer
{
public:
    static std::shared_ptr<Raytracer> create();
    explicit Raytracer(AK::Badge<Raytracer>);

    static std::shared_ptr<Raytracer> get_instance();

    void register_hittable(std::shared_ptr<Hittable> const& hittable);
    void unregister_hittable(std::shared_ptr<Hittable> const& hittable);

    void initialize(std::shared_ptr<Camera> const& camera);
    void render(std::shared_ptr<Camera> const& camera);

    void clear();

    void set_aspect_ratio(float const aspect_ratio);
    void set_image_width(i32 const image_width);
    void set_samples_per_pixel(i32 const samples_per_pixel);
    void set_max_depth(i32 const max_depth);
    void set_background_color(glm::vec3 const& background_color);

private:
    [[nodiscard]] Ray get_ray(i32 const i, i32 const k) const;
    bool hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const;

    [[nodiscard]] glm::vec3 ray_color(Ray const& ray, i32 const depth) const;

    [[nodiscard]] glm::vec3 sample_square() const;

    inline static std::weak_ptr<Raytracer> m_instance = {};

    inline static std::string output_directory = "./output/";
    inline static std::string output_file = "image.ppm";

    std::shared_ptr<Camera> m_camera = {};
    glm::vec3 m_camera_position_this_frame = {};

    i32 m_samples_per_pixel = 10;
    float m_pixel_samples_scale = 1.0f;

    i32 m_max_depth = 10;

    float m_aspect_ratio = 1.0f;

    glm::vec3 m_background_color = {};

    i32 m_image_width = 100;
    i32 m_image_height = 0;

    glm::vec3 m_pixel00_location = {};
    glm::vec3 m_pixel_delta_u = {};
    glm::vec3 m_pixel_delta_v = {};

    std::shared_ptr<BVHNode> m_root = {};

    std::vector<std::shared_ptr<Hittable>> m_hittables = {};

    AABB m_bbox = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)};
};
