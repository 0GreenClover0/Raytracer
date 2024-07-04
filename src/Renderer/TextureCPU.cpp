#include "TextureCPU.h"

#include "AK/Types.h"
#include "ResourceManager.h"

#include <cmath>

SolidColor::SolidColor(glm::vec3 const& color) : m_color(color)
{
}

SolidColor::SolidColor(float const red, float const green, float const blue) : m_color(glm::vec3(red, green, blue))
{
}

glm::vec3 SolidColor::value(float u, float v, glm::vec3 const& point) const
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

glm::vec3 CheckerTexture::value(float u, float v, glm::vec3 const& point) const
{
    i32 const x_int = static_cast<i32>(std::floor(m_inv_scale * point.x));
    i32 const y_int = static_cast<i32>(std::floor(m_inv_scale * point.y));
    i32 const z_int = static_cast<i32>(std::floor(m_inv_scale * point.z));

    bool const is_even = (x_int + y_int + z_int) % 2 == 0;

    return is_even ? m_even->value(u, v, point) : m_odd->value(u, v, point);
}

ImageTexture::ImageTexture(std::string const& path) : m_image(ResourceManager::get_instance().load_image(path))
{
}

ImageTexture::ImageTexture(std::shared_ptr<Image> const& image) : m_image(image)
{
}

glm::vec3 ImageTexture::value(float u, float v, glm::vec3 const& point) const
{
    // If we have no texture data, then return solid cyan as a debugging aid.
    if (m_image->height() <= 0)
        return {0.0f, 1.0f, 1.0f};

    // Clamp input texture coordinates to [0,1] x [1,0]
    u = glm::clamp(u, 0.0f, 1.0f);

    // NOTE: Original RTiOW code llipped V image coordinate, we are not doing that, for whatever reason the imaged is flipped somewhere else.
    v = glm::clamp(v, 0.0f, 1.0f);

    i32 i = static_cast<i32>(u * m_image->width());
    i32 k = static_cast<i32>(v * m_image->height());

    u8 const* pixel = m_image->pixel_data(i, k);

    float constexpr color_scale = 1.0f / 255.0f;

    return {color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]};
}

NoiseTexture::NoiseTexture(float const scale) : m_scale(scale)
{
}

glm::vec3 NoiseTexture::value(float u, float v, glm::vec3 const& point) const
{
    return glm::vec3(1.0f, 1.0f, 1.0f) * m_noise.noise(m_scale * point);
}
