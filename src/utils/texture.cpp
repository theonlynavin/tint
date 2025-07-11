#include "texture.hpp"

Tint::Texture::Texture(const Texture::Kind &kind, const Image::Format &format)
    : textureID(0), width(0), height(0), kind(kind), format(format)
{
    GL_CALL(glGenTextures(1, &textureID));
}

Tint::Texture::~Texture()
{
    if (textureID != 0)
    {
        GL_CALL(glDeleteTextures(1, &textureID));
    }
}

void Tint::Texture::Create(unsigned int w, unsigned int h, const Image& data)
{
    width = w;
    height = h;
    format = data.format;

    Bind();

    GLenum dataType;
    GLenum internalFormat;
    switch (data.format)
    {
        case Image::Format::RGB: 
            internalFormat = GL_RGB8; 
            dataType = GL_UNSIGNED_BYTE; 
            break;
        case Image::Format::RGB32F: 
            internalFormat = GL_RGB32F; 
            dataType = GL_FLOAT; 
            break;
        case Image::Format::RGBA: 
            internalFormat = GL_RGBA8; 
            dataType = GL_UNSIGNED_BYTE; 
            break;
        case Image::Format::RGBA32F: 
            internalFormat = GL_RGBA32F; 
            dataType = GL_FLOAT; 
            break;
        case Image::Format::Depth: 
            internalFormat = GL_DEPTH_COMPONENT; 
            dataType = GL_FLOAT; 
            break;
    }

    //TODO: Incorporate float stuff
    GL_CALL(glTexImage2D(static_cast<GLenum>(kind), 0, internalFormat, width, height, 0, 
        GL_RGBA, GL_UNSIGNED_BYTE, data.GetRawData().data()));

    SetWrapMode(WrapMode::Repeat, WrapMode::Repeat);
    SetFilterMode(FilterMode::Linear, FilterMode::Linear);

    Unbind();
}

void Tint::Texture::Create(unsigned int w, unsigned int h, Image::Format f)
{
    width = w;
    height = h;
    format = f;

    Bind();

    GLenum dataType;
    GLenum channelFormat;
    GLenum internalFormat;
    switch (format)
    {
        case Image::Format::RGB: 
            internalFormat = GL_RGB8; 
            channelFormat = GL_RGB;
            dataType = GL_UNSIGNED_BYTE; 
            break;
        case Image::Format::RGB32F: 
            internalFormat = GL_RGB32F; 
            channelFormat = GL_RGB;
            dataType = GL_FLOAT; 
            break;
        case Image::Format::RGBA: 
            internalFormat = GL_RGBA8; 
            channelFormat = GL_RGBA;
            dataType = GL_UNSIGNED_BYTE; 
            break;
        case Image::Format::RGBA32F: 
            internalFormat = GL_RGBA32F; 
            channelFormat = GL_RGBA;
            dataType = GL_FLOAT; 
            break;
        case Image::Format::Depth: 
            internalFormat = GL_DEPTH_COMPONENT; 
            channelFormat = GL_DEPTH_COMPONENT;
            dataType = GL_FLOAT; 
            break;
    }

    GL_CALL(glTexImage2D(static_cast<GLenum>(kind), 0, internalFormat, width, height, 0, 
        channelFormat, dataType, nullptr));

    SetWrapMode(WrapMode::Repeat, WrapMode::Repeat);
    SetFilterMode(FilterMode::Linear, FilterMode::Linear);

    Unbind();
}

void Tint::Texture::Bind(unsigned int textureUnit) const
{
    GL_CALL(glActiveTexture(GL_TEXTURE0 + textureUnit));
    GL_CALL(glBindTexture(static_cast<GLenum>(kind), textureID));
}

void Tint::Texture::Unbind() const
{
    GL_CALL(glBindTexture(static_cast<GLenum>(kind), 0));
}

void Tint::Texture::SetWrapMode(WrapMode s, WrapMode t)
{
    Bind();
    GL_CALL(glTexParameteri(static_cast<GLenum>(kind), GL_TEXTURE_WRAP_S, static_cast<GLint>(s)));
    GL_CALL(glTexParameteri(static_cast<GLenum>(kind), GL_TEXTURE_WRAP_T, static_cast<GLint>(t)));
    Unbind();
}

void Tint::Texture::SetFilterMode(FilterMode min, FilterMode mag)
{
    Bind();
    GL_CALL(glTexParameteri(static_cast<GLenum>(kind), GL_TEXTURE_MIN_FILTER, static_cast<GLint>(min)));
    GL_CALL(glTexParameteri(static_cast<GLenum>(kind), GL_TEXTURE_MAG_FILTER, static_cast<GLint>(mag)));
    Unbind();
}

void Tint::Texture::GenerateMipmaps()
{
    Bind();
    GL_CALL(glGenerateMipmap(static_cast<GLenum>(kind)));
    Unbind();
}

Tint::Image Tint::Texture::ReadData(Image::Format format) const
{
    Bind();

    Image img(width, height);
    img.format = format;
    int channels = 3;

    switch (format)
    {
        case Image::Format::RGB: channels = 3; break;
        case Image::Format::RGBA: channels = 4; break;
        case Image::Format::Depth: channels = 1; break;
    }

    std::vector<unsigned char> buffer(width * height * channels);

    // Bind texture and framebuffer
    GLuint fbo = 0;
    GL_CALL(glGenFramebuffers(1, &fbo));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, static_cast<GLenum>(kind), textureID, 0));

    // Read pixels
    GL_CALL(glReadBuffer(GL_COLOR_ATTACHMENT0));
    GL_CALL(glReadPixels(0, 0, width, height, static_cast<GLint>(format), GL_UNSIGNED_BYTE, buffer.data()));

    // Cleanup
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CALL(glDeleteFramebuffers(1, &fbo));

    img.SetRawData(buffer);

    Unbind();

    return img;
}
