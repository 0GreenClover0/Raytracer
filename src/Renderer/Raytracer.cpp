#include "Raytracer.h"

#include "AK/AK.h"
#include "AK/Math.h"
#include "AK/Types.h"
#include "BVHNode.h"
#include "Camera.h"
#include "Ray.h"

#include <glm/gtc/random.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/vec3.hpp>

#include <execution>
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

    // Resize bounding box
    m_bbox = AABB(m_bbox, hittable->bounding_box());
}

void Raytracer::unregister_hittable(std::shared_ptr<Hittable> const& hittable)
{
    AK::swap_and_erase(m_hittables, hittable);

    // FIXME: Make bounding box smaller?
}

void Raytracer::render(std::shared_ptr<Camera> const& camera)
{
    std::filesystem::path const directory = output_directory;

    if (!std::filesystem::exists(directory))
    {
        std::filesystem::create_directory(directory);
    }

    m_camera_position_this_frame = m_camera->get_position();

    std::vector<std::string> scanlines = {};
    scanlines.resize(m_image_height * m_image_width);

    auto range = std::views::iota(0, m_image_height);
    std::for_each(std::execution::par_unseq, range.begin(), range.end(), [&](size_t const k) {
        std::clog << "Scanline: " << (m_image_height - k) << '\n';

        i32 const index = static_cast<i32>(k) * m_image_width;

        for (size_t i = 0; i < m_image_width; ++i)
        {
            glm::vec3 pixel_color = {0.0f, 0.0f, 0.0f};

            for (i32 sample = 0; sample < m_samples_per_pixel; ++sample)
            {
                Ray ray = get_ray(static_cast<i32>(i), static_cast<i32>(k));
                pixel_color += ray_color(ray, m_max_depth);
            }

            glm::ivec3 const color_byte = AK::color_to_byte(pixel_color * m_pixel_samples_scale);
            scanlines[index + i] =
                std::to_string(color_byte.r) + ' ' + std::to_string(color_byte.g) + ' ' + std::to_string(color_byte.b) + '\n';
        }
    });

    std::ofstream output(output_directory + output_file);

    output << "P3\n" << m_image_width << ' ' << m_image_height << "\n255\n";

    for (auto const& line : scanlines)
    {
        output << line;
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

void Raytracer::set_samples_per_pixel(i32 const samples_per_pixel)
{
    m_samples_per_pixel = samples_per_pixel;
}

void Raytracer::set_max_depth(i32 const max_depth)
{
    m_max_depth = max_depth;
}

void Raytracer::set_background_color(glm::vec3 const& background_color)
{
    m_background_color = background_color;
}

Ray Raytracer::get_ray(i32 const i, i32 const k) const
{
    // Construct a camera ray originating from the origin and directed at randomly sampled
    // point around the pixel location i, k.

    glm::vec3 const offset = sample_square();
    glm::vec3 const pixel_sample = m_pixel00_location + ((static_cast<float>(i) + offset.x) * m_pixel_delta_u)
                                 + ((static_cast<float>(k) + offset.y) * m_pixel_delta_v);

    glm::vec3 const ray_direction = pixel_sample - m_camera_position_this_frame;

    return {m_camera_position_this_frame, ray_direction};
}

void Raytracer::initialize(std::shared_ptr<Camera> const& camera)
{
    m_camera = camera;

    m_pixel_samples_scale = 1.0f / static_cast<float>(m_samples_per_pixel);

    // Calculate the image height, and ensure that it's at least 1.
    m_image_height = static_cast<i32>(static_cast<float>(m_image_width) / m_aspect_ratio);
    m_image_height = (m_image_height < 1) ? 1 : m_image_height;

    // Determine viewport dimensions.
    float const focal_length = 3.47f;
    float const theta = m_camera->fov;
    float const h = glm::tan(theta / 2.0f);
    float const viewport_height = 2.0f * h * focal_length;
    float const viewport_width = viewport_height * (static_cast<float>(m_image_width) / static_cast<float>(m_image_height));

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    glm::vec3 const viewport_u = viewport_width * -m_camera->get_right();
    glm::vec3 const viewport_v = viewport_height * -m_camera->get_up();

    // Calculate the horizontal and vertical delta vectors from pixel to pixel.
    m_pixel_delta_u = viewport_u / static_cast<float>(m_image_width);
    m_pixel_delta_v = viewport_v / static_cast<float>(m_image_height);

    // Calculate the location of the upper left pixel.
    glm::vec3 const viewport_upper_left =
        m_camera->get_position() - focal_length * m_camera->get_front() - viewport_u / 2.0f - viewport_v / 2.0f;
    m_pixel00_location = viewport_upper_left + 0.5f * (m_pixel_delta_u + m_pixel_delta_v);

    m_root = std::make_shared<BVHNode>(m_hittables, 0, m_hittables.size());
}

bool Raytracer::hit(Ray const& ray, Interval const ray_t, HitRecord& hit_record) const
{
    HitRecord temp_record = {};
    bool hit_anything = false;

    if (m_root->hit(ray, Interval(ray_t.min, ray_t.max), temp_record))
    {
        hit_anything = true;
        hit_record = temp_record;
    }

    return hit_anything;
}

glm::vec3 Raytracer::ray_color(Ray const& ray, i32 const depth) const
{
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return {0.0f, 0.0f, 0.0f};

    HitRecord hit_record = {};

    // If the ray hits nothing, return the background color.
    if (!hit(ray, Interval(0.001f, AK::INFINITY_F), hit_record))
    {
        return m_background_color;
    }

    Ray scattered;
    glm::vec3 attenuation;
    glm::vec3 const emitted_color = hit_record.material->emit(hit_record.u, hit_record.v, hit_record.point);

    if (!hit_record.material->scatter(ray, hit_record, attenuation, scattered))
        return emitted_color;

    glm::vec3 const scattered_color = attenuation * ray_color(scattered, depth - 1);

    return emitted_color + scattered_color;
}

glm::vec3 Raytracer::sample_square() const
{
    // Returns the vector to a random point in the [-0.5, -0.5]-[+0.5, +0.5] unit square.
    return {glm::linearRand(0.0f, 1.0f) - 0.5f, glm::linearRand(0.0f, 1.0f) - 0.5f, 0.0f};
}
