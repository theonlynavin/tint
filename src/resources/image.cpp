#include "image.hpp"

#include <algorithm>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

Tint::Image::Image(size_t width, size_t height)
    : data(new glm::vec4[width * height]), width(width), height(height), format(Format::RGBA)
{
}

Tint::Image::Image(const Image &other)
    : width(other.width), height(other.height)
{
    data = new glm::vec4[width * height];
    memcpy(data, other.data, sizeof(glm::vec4) * width * height);
}

Tint::Image::~Image()
{
    delete[] data;
}

void Tint::Image::SetPixel(size_t x, size_t y, glm::vec4 color)
{
    data[y * width + x] = color;
}

glm::vec4 Tint::Image::GetPixel(size_t x, size_t y) const
{
    return data[y * width + x];
}

std::vector<unsigned char> Tint::Image::GetRawData() const
{
    std::vector<unsigned char> flat;
    switch (format)
    {
    case Format::RGB:
        flat.reserve(width * height * 3);    
        for (size_t i = 0; i < width; i++)
        {
            for (size_t j = 0; j < height; j++)
            {
                size_t pixelId = j * width + i;
                flat[3 * pixelId + 0] = (255 * data[pixelId].r);
                flat[3 * pixelId + 1] = (unsigned char)(255 * data[pixelId].g);
                flat[3 * pixelId + 2] = (unsigned char)(255 * data[pixelId].b);
            }
        }    
        break;
    
    default:
        break;
    }

    return flat;
}

void Tint::Image::SetRawData(const std::vector<unsigned char> &raw)
{
    switch (format)
    {
        case Image::Format::RGB:             
            for (size_t i = 0; i < width; i++)
            {
                for (size_t j = 0; j < height; j++)
                {
                    size_t pixelId = j * width + i;
                    data[pixelId].r = raw[3 * pixelId + 0] / 255.0;
                    data[pixelId].g = raw[3 * pixelId + 1] / 255.0;
                    data[pixelId].b = raw[3 * pixelId + 2] / 255.0;
                }
            }    
            break;

        case Image::Format::RGBA: 
            for (size_t i = 0; i < width; i++)
            {
                for (size_t j = 0; j < height; j++)
                {
                    size_t pixelId = j * width + i;
                    data[pixelId].r = raw[4 * pixelId + 0] / 255.0;
                    data[pixelId].g = raw[4 * pixelId + 1] / 255.0;
                    data[pixelId].b = raw[4 * pixelId + 2] / 255.0;
                    data[pixelId].a = raw[4 * pixelId + 3] / 255.0;
                }
            }    
            break;

        case Image::Format::Depth: 
            for (size_t i = 0; i < width; i++)
            {
                for (size_t j = 0; j < height; j++)
                {
                    size_t pixelId = j * width + i;
                    data[pixelId].r = raw[pixelId] / 255.0;
                }
            }    
            break;
    }
}

void Tint::Image::SaveAs(const std::string &filepath) const
{
    unsigned char* ucdata;
    int channels = 3;

    switch (format)
    {
    case Format::RGB:
        channels = 3;
        ucdata = new unsigned char[width * height * 3];
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
        break;

    case Format::RGBA:
        channels = 4;
        ucdata = new unsigned char[width * height * 4];
        for (size_t i = 0; i < width; i++)
        {
            for (size_t j = 0; j < height; j++)
            {
                size_t pixelId = j * width + i;
                ucdata[4 * pixelId + 0] = (unsigned char)(255 * data[pixelId].r);
                ucdata[4 * pixelId + 1] = (unsigned char)(255 * data[pixelId].g);
                ucdata[4 * pixelId + 2] = (unsigned char)(255 * data[pixelId].b);
                ucdata[4 * pixelId + 3] = (unsigned char)(255 * data[pixelId].a);
            }
        }
        break;
        
    case Format::Depth:
        channels = 1;
        ucdata = new unsigned char[width * height];
        for (size_t i = 0; i < width; i++)
        {
            for (size_t j = 0; j < height; j++)
            {
                size_t pixelId = j * width + i;
                ucdata[pixelId] = (unsigned char)(255 * data[pixelId].r);
            }
        }
        break;
    }

    std::string ext = filepath.substr(filepath.find_last_of('.')+1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "png")
        stbi_write_png(filepath.c_str(), width, height, channels, ucdata, 0);
    else if (ext == "jpg" || ext == "jpeg")
        stbi_write_jpg(filepath.c_str(), width, height, channels, ucdata, 100);
    else if (ext == "bmp")
        stbi_write_bmp(filepath.c_str(), width, height, channels, ucdata);

    delete[] ucdata;
}

Tint::Image Tint::Image::ReadFrom(const std::string &filepath)
{
    stbi_set_flip_vertically_on_load(true);

    int width, height, channels;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

    if (!data)
    {
        TRaiseError("Failed to load texture: " + filepath, "Image::ReadFrom");
    }

    Image img = Image(width, height);
    switch (channels)
    {
        case 1: img.format = Format::Depth;
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
            break;
        case 3: img.format = Format::RGB;
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
            break;
        case 4: img.format = Format::RGBA;
            for (size_t i = 0; i < width; i++)
            {
                for (size_t j = 0; j < height; j++)
                {
                    size_t pixelId = j * width + i;
                    img.data[pixelId].r = data[3 * pixelId + 0] / 255.0;
                    img.data[pixelId].g = data[3 * pixelId + 1] / 255.0;
                    img.data[pixelId].b = data[3 * pixelId + 2] / 255.0;
                    img.data[pixelId].a = data[3 * pixelId + 2] / 255.0;
                }
            }
            break;
        default:
            stbi_image_free(data);
            TRaiseError("Weird number of channels (" + std::to_string(channels) + ") while reading: " + filepath, "Image::ReadFrom");
    }


    stbi_image_free(data);

    return img;
}
