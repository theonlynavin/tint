#pragma once
#include "../utils/utils.hpp"

namespace Tint
{
    class Texture;
    
    namespace gl 
    {
        struct Material
        {
            glm::vec3 base_color;
            glm::vec3 specular_color;
            glm::vec3 sheen_color;
            glm::vec3 emission_color;
            uint base_color_texture;
            uint specular_color_texture;
            uint sheen_color_texture;
            uint emission_color_texture;

            float metallic;
            float roughness;
            float transmission;
            uint mrt_texture;
            
            float clearcoat;
            uint clearcoat_texture;
            float clearcoat_roughness;
            uint clearcoat_roughness_texture;
        };
    }

    class Material
    {
    private:
        /* data */
    public:

        struct Property
        {
            const enum class Kind
            {
                Float,
                Vector,
                Texture
            } kind;

            glm::vec3 vValue;
            float fValue;
            Texture* tValue;   
        
            void operator=(glm::vec3& v)
            {

            }
        };

        Material();
        ~Material();
        Property operator[](const std::string& s);
    };
} // namespace Tint
