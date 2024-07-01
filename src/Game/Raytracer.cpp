#include "Raytracer.h"

#include "AK/AK.h"
#include "AK/Math.h"
#include "AK/Types.h"
#include "Camera.h"
#include "Ray.h"

#include <glm/gtx/norm.hpp>
#include <glm/vec3.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

std::shared_ptr<Raytracer> Raytracer::create()
{
    auto raytracer = std::make_shared<Raytracer>(AK::Badge<Raytracer> {});

    if (!m_instance.expired())
    {
        Debug::log("Instance of Raytracer already exists in the scene.", DebugType::Error);
    }

    m_instance = raytracer;

    return raytracer;
}

Raytracer::Raytracer(AK::Badge<Raytracer>)
{
}

std::shared_ptr<Raytracer> Raytracer::get_instance()
{
    if (m_instance.expired())
        return nullptr;

    return m_instance.lock();
}

void Raytracer::register_hittable(std::shared_ptr<Hittable> const& hittable)
{
    m_hittables.emplace_back(hittable);
}

void Raytracer::unregister_hittable(std::shared_ptr<Hittable> const& hittable)
{
    AK::swap_and_erase(m_hittables, hittable);
}

void Raytracer::render(std::shared_ptr<Camera> const& camera) const
{
    std::filesystem::path const directory = output_directory;

    if (!std::filesystem::exists(directory))
    {
        std::filesystem::create_directory(directory);
    }

    std::ofstream output(output_directory + output_file);

    output << "P3\n" << m_image_width << ' ' << m_image_height << "\n255\n";

    glm::vec3 camera_position = m_camera->get_position();

    for (i32 k = 0; k < m_image_height; ++k)
    {
        std::clog << "\rScanlines remaining: " << (m_image_height - k) << ' ' << std::flush;
        for (i32 i = 0; i < m_image_width; ++i)
        {
            auto pixel_center = m_pixel00_location + (static_cast<float>(i) * m_pixel_delta_u) + (static_cast<float>(k) * m_pixel_delta_v);
            auto ray_direction = pixel_center - camera_position;

            Ray ray(camera_position, ray_direction);

            auto pixel_color = ray_color(ray);
            glm::ivec3 const color_byte = AK::color_to_byte(pixel_color);

            output << color_byte.r << ' ' << color_byte.g << ' ' << color_byte.b << '\n';
        }
    }

    std::clog << "\rDone.                 \n";
    output.close();
}

void Raytracer::clear()
{
    m_hittables.clear();
}

void Raytracer::set_aspect_ratio(float const aspect_ratio)
{
    m_aspect_ratio = aspect_ratio;
}

void Raytracer::set_image_width(i32 const image_width)
{
    m_image_width = image_width;
}

void Raytracer::initialize(std::shared_ptr<Camera> const& camera)
{
    m_camera = camera;

    // Calculate the image height, and ensure that it's at least 1.
    m_image_height = static_cast<i32>(static_cast<float>(m_image_width) / m_aspect_ratio);
    m_image_height = (m_image_height < 1) ? 1 : m_image_height;

    // Determine viewport dimensions.
    float constexpr focal_length = 1.0f;
    float constexpr viewport_height = 2.0f;
    float const viewport_width = viewport_height * (static_cast<float>(m_image_width) / m_image_height);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    glm::vec3 const viewport_u = {viewport_width, 0.0f, 0.0f};
    glm::vec3 constexpr viewport_v = {0.0f, -viewport_height, 0.0f};

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    m_pixel_delta_u = viewport_u / static_cast<float>(m_image_width);
    m_pixel_delta_v = viewport_v / static_cast<float>(m_image_height);

    // Calculate the location of the upper left pixel.
    glm::vec3 const viewport_upper_left =
        m_camera->get_position() - glm::vec3(0.0f, 0.0f, focal_length) - viewport_u / 2.0f - viewport_v / 2.0f;
    m_pixel00_location = viewport_upper_left + 0.5f * (m_pixel_delta_u + m_pixel_delta_v);
}

bool Raytracer::hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const
{
    HitRecord temp_record = {};
    bool hit_anything = false;

    float closest = ray_t.max;

    for (auto const& hittable : m_hittables)
    {
        if (hittable->hit(ray, Interval(ray_t.min, closest), temp_record))
        {
            hit_anything = true;
            closest = temp_record.t;
            hit_record = temp_record;
        }
    }

    return hit_anything;
}

glm::vec3 Raytracer::ray_color(Ray const& ray) const
{
    HitRecord hit_record = {};

    if (hit(ray, Interval(0.0f, AK::INFINITY_F), hit_record))
    {
        return 0.5f * (hit_record.normal + glm::vec3(1.0f, 1.0f, 1.0f));
    }

    glm::vec3 const unit_direction = glm::normalize(ray.direction());
    float const a = 0.5f * (unit_direction.y + 1.0f);
    return (1.0f - a) * glm::vec3(1.0f, 1.0f, 1.0f) + a * glm::vec3(0.5f, 0.7f, 1.0f);
}
