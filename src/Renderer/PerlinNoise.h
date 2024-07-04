#pragma once

#include "AK/Types.h"

#include <glm/vec3.hpp>

class PerlinNoise
{
public:
    PerlinNoise();
    ~PerlinNoise();

    [[nodiscard]] float noise(glm::vec3 const& point) const;

private:
    static float trilinear_interpolation(float c[2][2][2], float u, float v, float w);
    static i32* perlin_generate_permutation();
    static void permute(i32* p, i32 n);

    static i32 constexpr point_count = 256;

    float* m_rand_float = nullptr;

    i32* m_permutation_x = nullptr;
    i32* m_permutation_y = nullptr;
    i32* m_permutation_z = nullptr;
};
