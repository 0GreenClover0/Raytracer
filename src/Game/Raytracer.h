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

    void run(std::shared_ptr<Camera> const& camera) const;

    void clear();

private:
    bool hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const;

    [[nodiscard]] glm::vec3 ray_color(Ray const& ray) const;

    inline static std::weak_ptr<Raytracer> m_instance = {};

    inline static std::string output_directory = "./output/";
    inline static std::string output_file = "image.ppm";

    std::vector<std::shared_ptr<Hittable>> m_hittables = {};
};
