#include "PerlinNoise.h"

#include "AK/AK.h"

PerlinNoise::PerlinNoise()
{
    m_rand_float = new float[point_count];

    for (i32 i = 0; i < point_count; ++i)
    {
        m_rand_float[i] = AK::random_float_fast();
    }

    m_permutation_x = perlin_generate_permutation();
    m_permutation_y = perlin_generate_permutation();
    m_permutation_z = perlin_generate_permutation();
}

PerlinNoise::~PerlinNoise()
{
    delete[] m_rand_float;
    delete[] m_permutation_x;
    delete[] m_permutation_y;
    delete[] m_permutation_z;
}

float PerlinNoise::noise(glm::vec3 const& point) const
{
    auto const i = static_cast<i32>(4.0f * point.x) & 255;
    auto const k = static_cast<i32>(4.0f * point.y) & 255;
    auto const m = static_cast<i32>(4.0f * point.z) & 255;

    return m_rand_float[m_permutation_x[i] ^ m_permutation_y[k] ^ m_permutation_z[m]];
}

i32* PerlinNoise::perlin_generate_permutation()
{
    auto const p = new i32[point_count];

    for (i32 i = 0; i < point_count; ++i)
    {
        p[i] = i;
    }

    permute(p, point_count);

    return p;
}

void PerlinNoise::permute(i32* p, i32 n)
{
    for (i32 i = n - 1; i > 0; --i)
    {
        i32 const target = AK::random_int_fast(0, i);
        i32 const temp = p[i];
        p[i] = p[target];
        p[target] = temp;
    }
}
