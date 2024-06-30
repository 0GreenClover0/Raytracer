#pragma once

#include <string>

class Raytracer
{
public:
    void run();

    inline static std::string output_directory = "./output/";
    inline static std::string output_file = "image.ppm";
};
