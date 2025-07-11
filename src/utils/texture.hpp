#pragma once

#include "utils.hpp"
#include "../resources/image.hpp"

namespace Tint
{    
    class Texture
    {
    public:
        enum class WrapMode
        {
            Repeat = GL_REPEAT,
            MirroredRepeat = GL_MIRRORED_REPEAT,
            ClampToEdge = GL_CLAMP_TO_EDGE,
            ClampToBorder = GL_CLAMP_TO_BORDER
        };

        enum class FilterMode
        {
            Nearest = GL_NEAREST,
            Linear = GL_LINEAR,
            NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
            LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
            NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
            LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
        };

        enum class Kind
        {
            Image2D = GL_TEXTURE_2D,
            Image3D = GL_TEXTURE_3D,
            Buffer = GL_TEXTURE_BUFFER
        };

        Texture(const Texture::Kind& kind, const Image::Format& format);
        ~Texture();

        void Create(unsigned int width, unsigned int height, const Image& data);
        void Create(unsigned int width, unsigned int height, Image::Format format);
        void Bind(unsigned int textureUnit = 0) const;
        void Unbind() const;

        void SetWrapMode(WrapMode s, WrapMode t);
        void SetFilterMode(FilterMode min, FilterMode mag);
        void GenerateMipmaps();

        Image ReadData(Image::Format format) const;

        unsigned int GetWidth() const { return width; }
        unsigned int GetHeight() const { return height; }
        Image::Format GetFormat() const { return format; }
        unsigned int GetID() const { return textureID; }

    private:
        unsigned int textureID;
        Image::Format format;
        Texture::Kind kind;
        unsigned int width;
        unsigned int height;
    };

} // namespace Tint
