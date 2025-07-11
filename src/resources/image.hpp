#pragma once
#include "../utils/utils.hpp"

namespace Tint
{
    class Image
    {
    public:
        enum class Format
        {
            RGB = GL_RGB,
            RGB32F = GL_RGB32F,
            RGBA = GL_RGBA,
            RGBA32F = GL_RGBA32F,
            Depth = GL_DEPTH_COMPONENT
        };

        Image(size_t width, size_t height);
        Image(const Image& other);

        ~Image();
        
        void SetPixel(size_t x, size_t y, glm::vec4 color);
        glm::vec4 GetPixel(size_t x, size_t y) const;

        std::vector<unsigned char> GetRawData() const;
        void SetRawData(const std::vector<unsigned char>& data);

        unsigned int GetWidth() const { return width; }
        unsigned int GetHeight() const { return height; }
    
        void SaveAs(const std::string& filepath) const;
        static Image ReadFrom(const std::string& filepath);

        Format format;
    private:
        glm::vec4* data;
        const unsigned int width, height;
    };
} // namespace Tint
