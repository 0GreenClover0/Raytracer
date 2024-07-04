#pragma once

#include <memory>
#include <string>

#include <glm/vec3.hpp>

class Image;

class TextureCPU
{
public:
    virtual ~TextureCPU() = default;

    [[nodiscard]] virtual glm::vec3 value(float u, float v, glm::vec3 const& point) const = 0;
};

class SolidColor final : public TextureCPU
{
public:
    explicit SolidColor(glm::vec3 const& color);

    SolidColor(float const red, float const green, float const blue);

    [[nodiscard]] virtual glm::vec3 value(float u, float v, glm::vec3 const& point) const override;

private:
    glm::vec3 m_color;
};

class CheckerTexture final : public TextureCPU
{
public:
    CheckerTexture(float const scale, std::shared_ptr<TextureCPU> const& even, std::shared_ptr<TextureCPU> const& odd);

    CheckerTexture(float const scale, glm::vec3 const& color1, glm::vec3 const& color2);

    [[nodiscard]] virtual glm::vec3 value(float u, float v, glm::vec3 const& point) const override;

private:
    float m_inv_scale;
    std::shared_ptr<TextureCPU> m_even;
    std::shared_ptr<TextureCPU> m_odd;
};

class ImageTexture final : public TextureCPU
{
public:
    explicit ImageTexture(std::string const& path);
    explicit ImageTexture(std::shared_ptr<Image> const& image);

    [[nodiscard]] virtual glm::vec3 value(float u, float v, glm::vec3 const& point) const override;

private:
    std::shared_ptr<Image> m_image;
};
