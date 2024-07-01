#include "Raytracer.h"

#include "AK/AK.h"
#include "AK/Types.h"
#include "Camera.h"
#include "Ray.h"

#include <glm/vec3.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

void Raytracer::run(std::shared_ptr<Camera> const& camera)
{
    float constexpr aspect_ratio = 16.0f / 9.0f;

    i32 constexpr image_width = 400;

    // Calculate the image height, and ensure that it's at least 1.
    i32 image_height = static_cast<i32>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    // Camera, currently mostly hardcoded.
    float constexpr viewport_height = 2.0f;
    float viewport_width = viewport_height * (static_cast<float>(image_width) / image_height);
    float constexpr focal_length = 1.0f;
    glm::vec3 camera_center = camera->get_position();

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    glm::vec3 viewport_u = {viewport_width, 0.0f, 0.0f};
    glm::vec3 viewport_v = {0.0f, -viewport_height, 0.0f};

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    glm::vec3 pixel_delta_u = viewport_u / static_cast<float>(image_width);
    glm::vec3 pixel_delta_v = viewport_v / static_cast<float>(image_height);

    // Calculate the location of the upper left pixel.
    glm::vec3 viewport_upper_left = camera_center - glm::vec3(0.0f, 0.0f, focal_length) - viewport_u / 2.0f - viewport_v / 2.0f;
    glm::vec3 pixel00_location = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

    std::filesystem::path const directory = output_directory;

    if (!std::filesystem::exists(directory))
    {
        std::filesystem::create_directory(directory);
    }

    std::ofstream output(output_directory + output_file);

    output << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (i32 k = 0; k < image_height; ++k)
    {
        std::clog << "\rScanlines remaining: " << (image_height - k) << ' ' << std::flush;
        for (i32 i = 0; i < image_width; ++i)
        {
            auto pixel_center = pixel00_location + (static_cast<float>(i) * pixel_delta_u) + (static_cast<float>(k) * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;

            Ray ray(camera_center, ray_direction);

            auto pixel_color = ray_color(ray);
            glm::ivec3 const color_byte = AK::color_to_byte(pixel_color);

            output << color_byte.r << ' ' << color_byte.g << ' ' << color_byte.b << '\n';
        }
    }

    std::clog << "\rDone.                 \n";
    output.close();
}

glm::vec3 Raytracer::ray_color(Ray const& ray)
{
    float const hit = hit_sphere(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f, ray);
    if (hit > 0.0f)
    {
        glm::vec3 const normal = glm::normalize(ray.at(hit) - glm::vec3(0.0f, 0.0f, -1.0f));
        return 0.5f * glm::vec3(normal.r + 1.0f, normal.y + 1.0f, normal.z + 1.0f);
    }

    glm::vec3 const unit_direction = glm::normalize(ray.direction());
    float const a = 0.5f * (unit_direction.y + 1.0f);
    return (1.0f - a) * glm::vec3(1.0f, 1.0f, 1.0f) + a * glm::vec3(0.5f, 0.7f, 1.0f);
}

float Raytracer::hit_sphere(glm::vec3 const& center, float const radius, Ray const& ray)
{
    glm::vec3 const origin_center = center - ray.origin();

    float const a = glm::dot(ray.direction(), ray.direction());
    float const b = -2.0f * glm::dot(ray.direction(), origin_center);
    float const c = glm::dot(origin_center, origin_center) - radius * radius;

    float const discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0)
    {
        return -1.0f;
    }

    return (-b - glm::sqrt(discriminant)) / (2.0f * a);
}
