#include "utils.hpp"
#include <GL/glew.h>
#include <exception>

void Tint::gl::Initialize()
{
    if (glewInit() != GLEW_OK)
    {
        TRaiseError("Could not initialize GLEW!", "Tint::GLInitialize");
    }
}

void Tint::gl::CheckErrors(const char *file, int line)
{    
    std::cerr << "Tint has faced an error!\n";

    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) 
    {
        std::cerr << "[OpenGL Error] " << glewGetErrorString(errorCode) 
                  << " in " << file << " (" << line << ")" << std::endl;
    }
}

void Tint::TDebugMessage(const std::string &message)
{
#ifdef TINT_DEBUG
    std::cout << message << std::endl;
#endif
}

void Tint::TRaiseError(const std::string &message, const std::string &raiser)
{
    std::cerr << "Tint has faced an error!\n";
    std::cerr << "[" << raiser << "] " << message << std::endl;
    throw std::runtime_error(raiser);
}

void Tint::TRaiseWarning(const std::string &message, const std::string &raiser)
{
    std::cerr << "Tint is warning you!\n";
    std::cerr << "[" << raiser << "] " << message << std::endl;
}
