#pragma once

#include "AK/Badge.h"
#include "AK/Interval.h"
#include "Ray.h"
#include "Renderer/Hittable.h"

#include <glm/vec3.hpp>

#include <memory>
#include <string>
#include <vector>

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
    void render(std::shared_ptr<Camera> const& camera) const;

    void clear();

    void set_aspect_ratio(float const aspect_ratio);
    void set_image_width(i32 const image_width);

private:
    bool hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const;

    [[nodiscard]] glm::vec3 ray_color(Ray const& ray) const;

    inline static std::weak_ptr<Raytracer> m_instance = {};

    inline static std::string output_directory = "./output/";
    inline static std::string output_file = "image.ppm";

    std::shared_ptr<Camera> m_camera = {};

    float m_aspect_ratio = 1.0f;

    i32 m_image_width = 100;
    i32 m_image_height = 0;

    glm::vec3 m_pixel00_location = {};
    glm::vec3 m_pixel_delta_u = {};
    glm::vec3 m_pixel_delta_v = {};

    std::vector<std::shared_ptr<Hittable>> m_hittables = {};
};
