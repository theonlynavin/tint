#include "utils.hpp"

namespace Tint
{
    class Image
    {
    private:
        glm::vec3* data;
        const size_t width, height;
    public:
        Image(size_t width, size_t height);
        Image(Image& other);
        ~Image();
        
        void SetPixel(size_t x, size_t y, glm::vec3 color);
        glm::vec3 GetPixel(size_t x, size_t y) const;
    
        void SaveAs(const std::string& filepath) const;
        static Image ReadFrom(const std::string& filepath);
    };    
} // namespace Tint
