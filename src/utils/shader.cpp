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

void Tint::Shader::ReflectShaderVariables()
{
    GLint numUniforms = 0;
    GL_CALL(glGetProgramiv(programID, GL_ACTIVE_UNIFORMS, &numUniforms));

    char nameBuf[256];
    for (int i = 0; i < numUniforms; ++i) {
        GLenum type;
        GLint size;
        GL_CALL(glGetActiveUniform(programID, i, sizeof(nameBuf), nullptr, &size, &type, nameBuf));

        GLint loc = glGetUniformLocation(programID, nameBuf); GL_CALL();
        std::string name(nameBuf);
        uniformLocations[name] = loc;
    }

    GLint numUBOs = 0;
    GL_CALL(glGetProgramInterfaceiv(programID, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &numUBOs));

    for (int i = 0; i < numUBOs; ++i) {
        GLint nameLen = 0;
        GL_CALL(glGetProgramResourceiv(programID, GL_UNIFORM_BLOCK, i, 1,
            nullptr, 1, nullptr, &nameLen));

        std::vector<char> nameBuf(nameLen);
        GL_CALL(glGetProgramResourceName(programID, GL_UNIFORM_BLOCK, i, nameLen, nullptr, nameBuf.data()));
        std::string name(nameBuf.data());

        GLint binding = 0;
        GL_CALL(glGetProgramResourceiv(programID, GL_UNIFORM_BLOCK, i, 1,
            nullptr, 1, nullptr, &binding));

        uboLocations[name] = {static_cast<uint>(i), static_cast<uint>(binding)};
    }
    GLint numSSBOs = 0;
    GL_CALL(glGetProgramInterfaceiv(programID, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &numSSBOs));

    for (int i = 0; i < numSSBOs; ++i) {
        GLint nameLen = 0;
        GL_CALL(glGetProgramResourceiv(programID, GL_SHADER_STORAGE_BLOCK, i, 1,
            nullptr, 1, nullptr, &nameLen));

        std::vector<char> nameBuf(nameLen);
        GL_CALL(glGetProgramResourceName(programID, GL_SHADER_STORAGE_BLOCK, i, nameLen, nullptr, nameBuf.data()));
        std::string name(nameBuf.data());

        GLint binding = 0;
        GL_CALL(glGetProgramResourceiv(programID, GL_SHADER_STORAGE_BLOCK, i, 1,
            nullptr, 1, nullptr, &binding));

        ssboLocations[name] = {static_cast<uint>(i), static_cast<uint>(binding)};
    }
}

GLuint Tint::Shader::GetUniformLocation(const std::string &name) const
{  
    auto it = uniformLocations.find(name); 
    if (it == uniformLocations.end())
    {
        TRaiseWarning("Uniform '" + name + "' not found in shader!", "Shader");
        return -1;
    }
    return it->second;
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
        GL_CALL(glDeleteProgram(programID));
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
        GL_CALL(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success));
        if (!success) 
        {
            GLchar infoLog[1024];
            GL_CALL(glGetShaderInfoLog(shaderID, sizeof(infoLog), nullptr, infoLog));
            TRaiseError("Shader compilation failed:\n" + std::string(infoLog), "Shader::Compile");
        }

        GL_CALL(glAttachShader(programID, shaderID));
    }
    
    GL_CALL(glLinkProgram(programID));

    // Check for linking errors
    GLint success;
    GL_CALL(glGetProgramiv(programID, GL_LINK_STATUS, &success));
    if (!success) 
    {
        GLchar infoLog[1024];
        GL_CALL(glGetProgramInfoLog(programID, sizeof(infoLog), nullptr, infoLog));
        TRaiseError("Shader program linking failed:\n" + std::string(infoLog), "Shader::Compile");
    }

    // Update uniforms, TBOs, UBOs, SSBOs
    ReflectShaderVariables();
}

void Tint::Shader::SetFloat(const std::string &name, float value) const
{
    GL_CALL(glUniform1f(GetUniformLocation(name), value));
}

void Tint::Shader::SetFloat(const std::string &name, glm::vec2 value) const
{
    GL_CALL(glUniform2f(GetUniformLocation(name), value.x, value.y));
}

void Tint::Shader::SetFloat(const std::string &name, glm::vec3 value) const
{
    GL_CALL(glUniform3f(GetUniformLocation(name), value.x, value.y, value.z));
}

void Tint::Shader::SetFloat(const std::string &name, glm::vec4 value) const
{
    GL_CALL(glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w));
}

void Tint::Shader::SetInt(const std::string &name, int value) const
{
    GL_CALL(glUniform1i(GetUniformLocation(name), value));
}

void Tint::Shader::SetInt(const std::string &name, glm::ivec2 value) const
{
    GL_CALL(glUniform2i(GetUniformLocation(name), value.x, value.y));
}

void Tint::Shader::SetInt(const std::string &name, glm::ivec3 value) const
{
    GL_CALL(glUniform3i(GetUniformLocation(name), value.x, value.y, value.z));
}

void Tint::Shader::SetInt(const std::string &name, glm::ivec4 value) const
{
    GL_CALL(glUniform4i(GetUniformLocation(name), value.x, value.y, value.z, value.w));
}

void Tint::Shader::SetTexture(const std::string &name, const Tint::Texture &value) const
{
    value.Bind(GetUniformLocation(name));
}

void Tint::Shader::BindImage(const std::string &name, const Tint::Texture &value) const
{   
    GLint index;
    GL_CALL(glGetUniformiv(programID, GetUniformLocation(name), &index));
    GL_CALL(glBindImageTexture(index, value.GetID(), 0, GL_FALSE, 0, GL_READ_WRITE, static_cast<GLenum>(value.GetFormat())));

    value.Bind(index);
}

void Tint::Shader::BindUBO(const std::string &name, const Buffer& buffer) const
{
    auto it = uboLocations.find(name); 
    if (it == uboLocations.end())
    {
        TRaiseWarning("Block '" + name + "' not found in shader!", "Shader::BindUBO");
        return;
    }

    // it->second.first: index
    // it->second.second: bindingPoint
    GL_CALL(glUniformBlockBinding(programID, it->second.first, it->second.second));
    buffer.BindBase(it->second.second);
}

void Tint::Shader::BindSSBO(const std::string &name, const Buffer& buffer) const
{        
    auto it = ssboLocations.find(name); 
    if (it == ssboLocations.end())
    {
        TRaiseWarning("Block '" + name + "' not found in shader!", "Shader::BindSSBO");
        return;
    }
    
    // it->second.first: index
    // it->second.second: bindingPoint
    GL_CALL(glShaderStorageBlockBinding(programID, it->second.first, it->second.second));
    buffer.BindBase(it->second.second);
}

void Tint::Shader::Use() const
{
    GL_CALL(glUseProgram(programID));
}
