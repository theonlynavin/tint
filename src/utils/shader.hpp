#pragma once
#include "utils.hpp"
#include <unordered_map>

namespace Tint
{
    class Buffer;
    class Texture;

    class Shader
    {
    public:
        enum class ShaderType
        {
            VERTEX,
            FRAGMENT,
            COMPUTE
        };

        Shader(const std::vector<std::pair<std::string, Tint::Shader::ShaderType>> &shaderpaths);
        ~Shader();
        
        void Compile();
        
        void SetFloat(const std::string& name, float value) const;
        void SetFloat(const std::string& name, glm::vec2 value) const;
        void SetFloat(const std::string& name, glm::vec3 value) const;
        void SetFloat(const std::string& name, glm::vec4 value) const;
        void SetInt(const std::string& name, int value) const;
        void SetInt(const std::string& name, glm::ivec2 value) const;
        void SetInt(const std::string& name, glm::ivec3 value) const;
        void SetInt(const std::string& name, glm::ivec4 value) const;
        void SetTexture(const std::string &name, const Texture &value) const;

        void BindImage(const std::string &name, const Tint::Texture& value) const;
        void BindUBO(const std::string &name, const Buffer &buffer) const;
        void BindSSBO(const std::string &name, const Buffer &buffer) const;

        void Use() const;

    private:
        uint programID;
        std::vector<uint> shaderIDs;
        std::vector<std::pair<Tint::Shader::ShaderType, std::string>> mainSource;
        std::unordered_map<std::string, std::string> shaderSources;
        std::unordered_map<std::string, uint> uniformLocations;
        std::unordered_map<std::string, std::pair<uint, uint>> uboLocations;
        std::unordered_map<std::string, std::pair<uint, uint>> ssboLocations;
        bool LoadRecursive(const std::string& filepath, std::string& output, int depth);
        void ReflectShaderVariables();
        uint GetUniformLocation(const std::string& name) const;
    };
} // namespace Tint
