#pragma once
#include "utils.hpp"

namespace Tint
{
    class Texture;
    
    class Buffer
    {
    private:
        unsigned int bufferID;
        unsigned int bufferType;
        size_t bufferSize;
        
    public:
        enum BufferType
        {
            UBO = GL_UNIFORM_BUFFER,          // GL_UNIFORM_BUFFER
            SSBO = GL_SHADER_STORAGE_BUFFER,  // GL_SHADER_STORAGE_BUFFER
            TBO = GL_TEXTURE_BUFFER           // GL_TEXTURE_BUFFER
        };
        
        Buffer();
        ~Buffer();
        
        void Allocate(size_t size, BufferType type);
        void Store(const void* data, size_t size, size_t offset = 0);

        void Bind() const;
        void BindBase(unsigned int bindingPoint) const;
        void Attach(const Texture& texture) const;
        void Unbind() const;

        uint GetID() const;

        void* Map();
        void Unmap();
    };    
} // namespace Tint