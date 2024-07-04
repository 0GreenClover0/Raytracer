#include "PerlinNoise.h"

#include "AK/AK.h"

#include <glm/gtc/random.hpp>

PerlinNoise::PerlinNoise()
{
    m_rand_float = new float[point_count];

    for (i32 i = 0; i < point_count; ++i)
    {
        m_random_vectors[i] = glm::normalize(glm::linearRand(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f)));
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
    float const x_floor = std::floor(point.x);
    float const y_floor = std::floor(point.y);
    float const z_floor = std::floor(point.z);

    auto const u = point.x - x_floor;
    auto const v = point.y - y_floor;
    auto const w = point.z - z_floor;

    auto const i = static_cast<i32>(x_floor);
    auto const k = static_cast<i32>(y_floor);
    auto const m = static_cast<i32>(z_floor);

    glm::vec3 c[2][2][2];

    for (i32 di = 0; di < 2; ++di)
    {
        for (i32 dk = 0; dk < 2; ++dk)
        {
            for (i32 dm = 0; dm < 2; ++dm)
            {
                c[di][dk][dm] =
                    m_random_vectors[m_permutation_x[(i + di) & 255] ^ m_permutation_y[(k + dk) & 255] ^ m_permutation_z[(m + dm) & 255]];
            }
        }
    }

    return perlin_interpolation(c, u, v, w);
}

float PerlinNoise::perlin_interpolation(glm::vec3 const c[2][2][2], float const u, float const v, float const w)
{
    float const uu = u * u * (3 - 2 * u);
    float const vv = v * v * (3 - 2 * v);
    float const ww = w * w * (3 - 2 * w);
    float accumulator = 0.0f;

    for (i32 i = 0; i < 2; ++i)
    {
        for (i32 k = 0; k < 2; ++k)
        {
            for (i32 m = 0; m < 2; ++m)
            {
                auto const weight_v = glm::vec3(u - i, v - k, w - m);
                accumulator += (i * uu + (1 - i) * (1 - uu)) * (k * vv + (1 - k) * (1 - vv)) * (m * ww + (1 - m) * (1 - ww))
                             * glm::dot(c[i][k][m], weight_v);
            }
        }
    }

    return accumulator;
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
