#pragma once

#include "AK/Types.h"

#include <array>
#include <glm/vec3.hpp>

class PerlinNoise
{
public:
    PerlinNoise();
    ~PerlinNoise();

    [[nodiscard]] float noise(glm::vec3 const& point) const;
    [[nodiscard]] float turbulence(glm::vec3 const& point, i32 const depth) const;

private:
    static float perlin_interpolation(glm::vec3 const c[2][2][2], float const u, float const v, float const w);
    static i32* perlin_generate_permutation();
    static void permute(i32* p, i32 n);

    static i32 constexpr point_count = 256;

    std::array<glm::vec3, point_count> m_random_vectors = {};
    float* m_rand_float = nullptr;

    i32* m_permutation_x = nullptr;
    i32* m_permutation_y = nullptr;
    i32* m_permutation_z = nullptr;
};
