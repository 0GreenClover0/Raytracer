#pragma once

#include "Ray.h"

#include <glm/vec3.hpp>

#include <memory>
#include <string>

class Camera;

class Raytracer
{
public:
    void run(std::shared_ptr<Camera> const& camera);

private:
    static glm::vec3 ray_color(Ray const& ray);
    static float hit_sphere(glm::vec3 const& center, float const radius, Ray const& ray);

    inline static std::string output_directory = "./output/";
    inline static std::string output_file = "image.ppm";
};
