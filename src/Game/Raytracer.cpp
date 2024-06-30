#include "Raytracer.h"

#include "AK/Types.h"

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
            float const r = static_cast<float>(i) / (image_width - 1);
            float const g = static_cast<float>(k) / (image_height - 1);
            float constexpr b = 0.0;

            i32 const ir = static_cast<i32>(255.99f * r);
            i32 const ig = static_cast<i32>(255.99f * g);
            i32 constexpr ib = static_cast<i32>(255.99f * b);

            output << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }

    std::clog << "\rDone.                 \n";
}
