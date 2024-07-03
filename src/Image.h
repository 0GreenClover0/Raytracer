#pragma once

#include "AK/Types.h"

#include <string>

class Image
{
public:
    Image() = default;

    explicit Image(std::string const& path);

    ~Image();

    bool load(std::string const& path);

    [[nodiscard]] i32 width() const;
    [[nodiscard]] i32 height() const;

    [[nodiscard]] u8 const* pixel_data(i32 const x, i32 const y) const;

private:
    static i32 clamp(i32 const x, i32 const low, i32 const high);
    static u8 float_to_byte(float const value);

    void convert_to_bytes();

    inline static i32 bytes_per_pixel = 3;

    float* m_float_data = nullptr;
    u8* m_byte_data = nullptr;
    i32 m_image_width = 0;
    i32 m_image_height = 0;
    i32 m_bytes_per_scanline = 0;
};
