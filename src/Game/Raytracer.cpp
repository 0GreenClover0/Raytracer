#include "Raytracer.h"

#include "AK/AK.h"
#include "AK/Types.h"

#include <glm/vec3.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

void Raytracer::run()
{
    i32 constexpr image_width = 256;
    i32 constexpr image_height = 256;

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
}
