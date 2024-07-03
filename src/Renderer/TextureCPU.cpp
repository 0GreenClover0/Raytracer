#include "TextureCPU.h"

#include "AK/Types.h"

#include <cmath>

SolidColor::SolidColor(glm::vec3 const& color) : m_color(color)
{
}

SolidColor::SolidColor(float const red, float const green, float const blue) : m_color(glm::vec3(red, green, blue))
{
}

glm::vec3 SolidColor::value(float const u, float const v, glm::vec3 const& point) const
{
    return m_color;
}

CheckerTexture::CheckerTexture(float const scale, std::shared_ptr<TextureCPU> const& even, std::shared_ptr<TextureCPU> const& odd)
    : m_inv_scale(1.0f / scale), m_even(even), m_odd(odd)
{
}

CheckerTexture::CheckerTexture(float const scale, glm::vec3 const& color1, glm::vec3 const& color2)
    : m_inv_scale(1.0f / scale), m_even(std::make_shared<SolidColor>(color1)), m_odd(std::make_shared<SolidColor>(color2))
{
}

glm::vec3 CheckerTexture::value(float const u, float const v, glm::vec3 const& point) const
{
    i32 const x_int = static_cast<i32>(std::floor(m_inv_scale * point.x));
    i32 const y_int = static_cast<i32>(std::floor(m_inv_scale * point.y));
    i32 const z_int = static_cast<i32>(std::floor(m_inv_scale * point.z));

    bool const is_even = (x_int + y_int + z_int) % 2 == 0;

    return is_even ? m_even->value(u, v, point) : m_odd->value(u, v, point);
}
