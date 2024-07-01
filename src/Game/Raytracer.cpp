#include "Raytracer.h"

#include "AK/AK.h"
#include "AK/Types.h"

#include <glm/vec3.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

void Raytracer::run()
{
    float constexpr aspect_ratio = 16.0f / 9.0f;

    i32 constexpr image_width = 400;
    i32 image_height = static_cast<i32>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;

    float constexpr viewport_height = 2.0f;
    float viewport_width = viewport_height * (static_cast<float>(image_width) / image_height);

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
            auto pixel_color = glm::vec3(static_cast<float>(i) / (image_width - 1), static_cast<float>(k) / (image_height - 1), 0.0f);

            glm::ivec3 const color_byte = AK::color_to_byte(pixel_color);

            output << color_byte.r << ' ' << color_byte.g << ' ' << color_byte.b << '\n';
        }
    }

    std::clog << "\rDone.                 \n";
    output.close();
}
