#pragma once

#include <memory>

#include <glm/vec3.hpp>

class TextureCPU
{
public:
    virtual ~TextureCPU() = default;

    [[nodiscard]] virtual glm::vec3 value(float const u, float const v, glm::vec3 const& point) const = 0;
};

class SolidColor final : public TextureCPU
{
public:
    explicit SolidColor(glm::vec3 const& color);

    SolidColor(float const red, float const green, float const blue);

    [[nodiscard]] virtual glm::vec3 value(float const u, float const v, glm::vec3 const& point) const override;

private:
    glm::vec3 m_color;
};

class CheckerTexture final : public TextureCPU
{
public:
    CheckerTexture(float const scale, std::shared_ptr<TextureCPU> const& even, std::shared_ptr<TextureCPU> const& odd);

    CheckerTexture(float const scale, glm::vec3 const& color1, glm::vec3 const& color2);

    [[nodiscard]] virtual glm::vec3 value(float const u, float const v, glm::vec3 const& point) const override;

private:
    float m_inv_scale;
    std::shared_ptr<TextureCPU> m_even;
    std::shared_ptr<TextureCPU> m_odd;
};
