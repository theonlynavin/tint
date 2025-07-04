#include "utils.hpp"

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
        Image(Image& other);

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

        // Disallow copying
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        // Allow moving
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

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
