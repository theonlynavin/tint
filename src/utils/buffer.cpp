#include "buffer.hpp"
#include "image.hpp"
#include <GL/glew.h>

Tint::Buffer::Buffer()
    : bufferID(0), bufferType(0), bufferSize(0)
{
    GL_CALL(glGenBuffers(1, &bufferID));
    if (bufferID == 0)
    {
        TRaiseError("Failed to generate buffer!", "Buffer");
    }
}

Tint::Buffer::~Buffer()
{
    if (bufferID != 0)
    {
        GL_CALL(glDeleteBuffers(1, &bufferID));
    }
}

uint Tint::Buffer::GetID() const
{
    return bufferID;
}

void Tint::Buffer::Allocate(size_t size, BufferType type)
{
    if (bufferID == 0)
    {
        TRaiseError("Buffer not initialized!", "Buffer::Allocate");
    }

    bufferType = type;
    bufferSize = size;

    GL_CALL(glBindBuffer(bufferType, bufferID));
    GL_CALL(glBufferData(bufferType, size, nullptr, GL_STATIC_DRAW));
    GL_CALL(glBindBuffer(bufferType, 0));
}

void Tint::Buffer::Store(const void *data, size_t size, size_t offset)
{
    if (bufferID == 0)
    {
        TRaiseError("Buffer not initialized!", "Buffer::Store");
    }
    if (offset + size > bufferSize)
    {
        TRaiseError("Buffer store operation out of range!", "Buffer::Store");
    }

    GL_CALL(glBindBuffer(bufferType, bufferID));
    GL_CALL(glBufferSubData(bufferType, offset, size, data));
    GL_CALL(glBindBuffer(bufferType, 0));
}

void Tint::Buffer::Bind() const
{
    if (bufferID == 0)
    {
        TRaiseWarning("Buffer not initialized!", "Buffer::Bind");
    }
    GL_CALL(glBindBuffer(bufferType, bufferID));
}

void Tint::Buffer::BindBase(unsigned int bindingPoint) const
{
    if (bufferID == 0)
    {
        TRaiseWarning("Buffer not initialized!", "Buffer::BindBase");
    }
    GL_CALL(glBindBufferBase(bufferType, bindingPoint, bufferID));
}

void Tint::Buffer::Attach(const Tint::Texture& texture) const
{
    Bind();
    texture.Bind();
    GL_CALL(glTexBuffer(bufferType, static_cast<int>(texture.GetFormat()), bufferID));
    texture.Unbind();
    Unbind();
}

void Tint::Buffer::Unbind() const
{
    GL_CALL(glBindBuffer(bufferType, 0));
}

void *Tint::Buffer::Map()
{
    if (bufferID == 0)
    {
        TRaiseError("Buffer not initialized!", "Buffer::Map");
    }
    Bind();
    void *ptr = glMapBuffer(bufferType, GL_READ_WRITE); GL_CALL();
    if (ptr == nullptr)
    {
        TRaiseError("Failed to map buffer!", "Buffer::Map");
    }
    return ptr;
}

void Tint::Buffer::Unmap()
{
    if (bufferID == 0)
    {
        TRaiseError("Buffer not initialized!", "Buffer::Unmap");
    }
    if (!glUnmapBuffer(bufferType))
    {
        TRaiseError("Buffer data corrupted during unmap!", "Buffer::Unmap");
    }
    Unbind();
}