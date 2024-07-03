#include "Image.h"

#include <stb_image.h>

Image::Image(std::string const& path)
{
    bool const result = load(path);

    if (!result)
    {
        std::cout << "Could not load an image: " << path << "\n";
    }
}

Image::~Image()
{
    delete[] m_byte_data;

    stbi_image_free(m_float_data);
}

bool Image::load(std::string const& path)
{
    // Loads the linear (gamma=1) image data from the given file name. Returns true if the
    // load succeeded. The resulting data buffer contains the three [0.0, 1.0]
    // floating-point values for the first pixel (red, then green, then blue). Pixels are
    // contiguous, going left to right for the width of the image, followed by the next row
    // below, for the full height of the image.

    auto n = bytes_per_pixel;
    m_float_data = stbi_loadf(path.c_str(), &m_image_width, &m_image_height, &n, bytes_per_pixel);

    if (m_float_data == nullptr)
    {
        return false;
    }

    m_bytes_per_scanline = m_image_width * bytes_per_pixel;
    convert_to_bytes();
    return true;
}

i32 Image::width() const
{
    return (m_float_data == nullptr) ? 0 : m_image_width;
}

i32 Image::height() const
{
    return (m_float_data == nullptr) ? 0 : m_image_height;
}

u8 const* Image::pixel_data(i32 x, i32 y) const
{
    // Return the address of the three RGB bytes of the pixel at x,y. If there is no image
    // data, returns magenta.
    static u8 magenta[] = {255, 0, 255};
    if (m_byte_data == nullptr)
        return magenta;

    x = clamp(x, 0, m_image_width);
    y = clamp(y, 0, m_image_height);

    return m_byte_data + y * m_bytes_per_scanline + x * bytes_per_pixel;
}

i32 Image::clamp(i32 const x, i32 const low, i32 const high)
{
    // Return the value clamped to the range [low, high).
    if (x < low)
        return low;

    if (x < high)
        return x;

    return high - 1;
}

u8 Image::float_to_byte(float const value)
{
    if (value <= 0.0f)
        return 0.0f;

    if (1.0f <= value)
        return 255;

    return static_cast<u8>(256.0f * value);
}

void Image::convert_to_bytes()
{
    // Convert the linear floating point pixel data to bytes, storing the resulting byte
    // data in the `bdata` member.

    i32 const total_bytes = m_image_width * m_image_height * bytes_per_pixel;
    m_byte_data = new u8[total_bytes];

    // Iterate through all pixel components, converting from [0.0, 1.0] float values to
    // unsigned [0, 255] byte values.

    u8* byte_ptr = m_byte_data;
    float* float_ptr = m_float_data;

    for (auto i = 0; i < total_bytes; i++, float_ptr++, byte_ptr++)
    {
        *byte_ptr = float_to_byte(*float_ptr);
    }
}
