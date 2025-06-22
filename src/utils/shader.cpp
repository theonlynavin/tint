#include "shader.hpp"
#include "buffer.hpp"
#include "image.hpp"
#include <fstream>
#include <filesystem>
#include <GL/glew.h>

bool Tint::Shader::LoadRecursive(const std::string &filepath, std::string& output, int depth)
{
    if (depth > 32) 
    {
        TRaiseError("Include depth exceeded maximum limit (32) in file: " + filepath, "Shader Loader");
        return false;
    }

    // Exit early if already loaded file
    auto it = shaderSources.find(filepath);
    if (it != shaderSources.end()) 
    {
        output = it->second;
        return true;
    }

    // Try opening the file
    std::ifstream file(filepath);
    if (!file.is_open()) 
    {
        TRaiseError("Could not open shader file: " + filepath, "Shader Loader");
        return false;
    }

    std::filesystem::path parentPath = std::filesystem::path(filepath).parent_path();

    std::stringstream combinedSource;
    std::string line;
    size_t lineNumber = 0;

    while (std::getline(file, line)) 
    {
        lineNumber++;
        
        // Check for #include directive
        if (line.find("#include") == 0) 
        {
            size_t start = line.find('"');
            size_t end = line.rfind('"');
            if (start == std::string::npos || end == std::string::npos || start == end) 
            {
                TRaiseError("Malformed #include directive in: " + filepath + " at line " 
                    + std::to_string(lineNumber), "Shader::Load");
                return false;
            }

            std::string includeFile = line.substr(start + 1, end - start - 1);
            std::filesystem::path includePath = parentPath / includeFile;

            // Recursively resolve includes in the included file
            std::string includedSource;
            if (!LoadRecursive(includePath.string(), includedSource, depth + 1)) 
            {
                return false;
            }

            combinedSource << includedSource << "\n";
        } 
        else 
        {
            combinedSource << line << "\n";
        }
    }

    output = combinedSource.str();
    shaderSources[filepath] = output;

    return true;
}

GLuint Tint::Shader::GetUniformLocation(const std::string &name) const
{   
    GLint location = glGetUniformLocation(programID, name.c_str()); GL_CALL();
    if (location == -1) 
    {
        TRaiseWarning("Uniform '" + name + "' not found in shader!", "Shader");
    }
    return location;
}

Tint::Shader::Shader(const std::vector<std::pair<std::string, Tint::Shader::ShaderType>> &shaderpaths)
{
    for (size_t i = 0; i < shaderpaths.size(); i++)
    {
        const std::string& filepath = shaderpaths[i].first;
        const Tint::Shader::ShaderType& kind = shaderpaths[i].second;
        std::string src;

        if (!LoadRecursive(filepath, src, 0))
        {
            TRaiseError("Shader '" + filepath + "' loading failure!", "Shader::Shader");
        }

        mainSource.push_back(std::make_pair(kind, src));        
    }
}

Tint::Shader::~Shader()
{
    if (programID != 0)
    {
        glDeleteProgram(programID);
    }
}

void Tint::Shader::Compile()
{
    programID = glCreateProgram(); GL_CALL();    

    for (size_t i = 0; i < mainSource.size(); i++)
    {
        GLuint shaderType;
        switch (mainSource[i].first)
        {
        case ShaderType::VERTEX:
            shaderType = GL_VERTEX_SHADER;
            break;
        case ShaderType::FRAGMENT:
            shaderType = GL_FRAGMENT_SHADER;
            break;
        case ShaderType::COMPUTE:
            shaderType = GL_COMPUTE_SHADER;
            break;
        }   

        GLuint shaderID = glCreateShader(shaderType); GL_CALL();
        shaderIDs.push_back(shaderID);

        const char* src = mainSource[i].second.c_str();
        GL_CALL(glShaderSource(shaderID, 1, &src, nullptr));
        GL_CALL(glCompileShader(shaderID));

        // Check for compilation errors
        GLint success;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success) 
        {
            GLchar infoLog[1024];
            glGetShaderInfoLog(shaderID, sizeof(infoLog), nullptr, infoLog);
            TRaiseError("Shader compilation failed:\n" + std::string(infoLog), "Shader::Compile");
        }

        GL_CALL(glAttachShader(programID, shaderID));
    }
    
    GL_CALL(glLinkProgram(programID));
    // Check for linking errors
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) 
    {
        GLchar infoLog[1024];
        glGetProgramInfoLog(programID, sizeof(infoLog), nullptr, infoLog);
        TRaiseError("Shader program linking failed:\n" + std::string(infoLog), "Shader::Compile");
    }
}

void Tint::Shader::SetFloat(const std::string &name, float value) const
{
    glUniform1f(GetUniformLocation(name), value);
}

void Tint::Shader::SetFloat(const std::string &name, glm::vec2 value) const
{
    glUniform2f(GetUniformLocation(name), value.x, value.y);
}

void Tint::Shader::SetFloat(const std::string &name, glm::vec3 value) const
{
    glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
}

void Tint::Shader::SetFloat(const std::string &name, glm::vec4 value) const
{
    glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Tint::Shader::SetInt(const std::string &name, int value) const
{
    glUniform1i(GetUniformLocation(name), value);
}

void Tint::Shader::SetInt(const std::string &name, glm::ivec2 value) const
{
    glUniform2i(GetUniformLocation(name), value.x, value.y);
}

void Tint::Shader::SetInt(const std::string &name, glm::ivec3 value) const
{
    glUniform3i(GetUniformLocation(name), value.x, value.y, value.z);
}

void Tint::Shader::SetInt(const std::string &name, glm::ivec4 value) const
{
    glUniform4i(GetUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Tint::Shader::SetImage(const std::string &name, const Tint::Texture &value) const
{   
    GLint index;
    glGetUniformiv(programID, GetUniformLocation(name), &index);
    glBindImageTexture(index, value.GetID(), 0, GL_FALSE, 0, GL_READ_WRITE, static_cast<GLenum>(value.GetFormat()));
}

void Tint::Shader::BindUBO(const std::string &name, Buffer& buffer, uint bindingPoint) const
{
    GLint index = glGetUniformBlockIndex(programID, name.c_str());
    if (index == GL_INVALID_INDEX) 
    {
        TRaiseWarning("Block '" + name + "' not found in shader!", "Shader::BindUBO");
    }
    glUniformBlockBinding(programID, index, bindingPoint);
    buffer.BindBase(bindingPoint);
}

void Tint::Shader::BindSSBO(const std::string &name, Buffer& buffer, uint bindingPoint) const
{        
    GLint index = glGetProgramResourceIndex(programID, GL_SHADER_STORAGE_BLOCK, name.c_str());
    if (index == GL_INVALID_INDEX) 
    {
        TRaiseWarning("Block '" + name + "' not found in shader!", "Shader::BindSSBO");
    }
    glShaderStorageBlockBinding(programID, index, bindingPoint);
    buffer.BindBase(bindingPoint);
}

void Tint::Shader::Use() const
{
    glUseProgram(programID);
}
