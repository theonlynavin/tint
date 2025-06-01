#include "image.hpp"

#include <algorithm>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

Tint::Image::Image(size_t width, size_t height)
    : data(new glm::vec3[width * height]), width(width), height(height)
{
}

Tint::Image::Image(Image &other)
    : width(other.width), height(other.height)
{
    data = new glm::vec3[width * height];
    memcpy(data, other.data, sizeof(glm::vec3) * width * height);
}

Tint::Image::~Image()
{
    delete[] data;
}

void Tint::Image::SetPixel(size_t x, size_t y, glm::vec3 color)
{
    data[y * width + x] = color;
}

glm::vec3 Tint::Image::GetPixel(size_t x, size_t y) const
{
    return data[y * width + x];
}

void Tint::Image::SaveAs(const std::string &filepath) const
{
    unsigned char* ucdata = new unsigned char[width * height * 3];
    for (size_t i = 0; i < width; i++)
    {
        for (size_t j = 0; j < height; j++)
        {
            size_t pixelId = j * width + i;
            ucdata[3 * pixelId + 0] = (unsigned char)(255 * data[pixelId].r);
            ucdata[3 * pixelId + 1] = (unsigned char)(255 * data[pixelId].g);
            ucdata[3 * pixelId + 2] = (unsigned char)(255 * data[pixelId].b);
        }
    }

    std::string ext = filepath.substr(filepath.find_last_of('.')+1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "png")
        stbi_write_png(filepath.c_str(), width, height, 3, ucdata, 0);
    else if (ext == "jpg" || ext == "jpeg")
        stbi_write_jpg(filepath.c_str(), width, height, 3, ucdata, 100);
    else if (ext == "bmp")
        stbi_write_bmp(filepath.c_str(), width, height, 3, ucdata);

    delete[] ucdata;
}

Tint::Image Tint::Image::ReadFrom(const std::string &filepath)
{
    int width, height, channels;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 3);

    Image img = Image(width, height);

    for (size_t i = 0; i < width; i++)
    {
        for (size_t j = 0; j < height; j++)
        {
            size_t pixelId = j * width + i;
            img.data[pixelId].r = data[3 * pixelId + 0] / 255.0;
            img.data[pixelId].g = data[3 * pixelId + 1] / 255.0;
            img.data[pixelId].b = data[3 * pixelId + 2] / 255.0;
        }
    }

    delete[] data;

    return img;
}
