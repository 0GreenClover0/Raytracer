#pragma once

#include "AK/Badge.h"
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

    void run(std::shared_ptr<Camera> const& camera);

    void clear();

private:
    bool hit(Ray const& ray, float const ray_tmin, float const ray_tmax, HitRecord& hit_record) const;

    static glm::vec3 ray_color(Ray const& ray);
    static float hit_sphere(glm::vec3 const& center, float const radius, Ray const& ray);

    inline static std::weak_ptr<Raytracer> m_instance = {};

    inline static std::string output_directory = "./output/";
    inline static std::string output_file = "image.ppm";

    std::vector<std::shared_ptr<Hittable>> m_hittables = {};
};
