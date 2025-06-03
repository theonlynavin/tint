#include "../utils/frame.hpp"
#include "../intersection/triangle.hpp"
#include "../intersection/aabb.hpp"
#include "../material/material.hpp"
#include <vector>
#include <string>

namespace Tint
{
    struct Ray;
    class Material;
    
    class Object
    {
    private:
        std::vector<Triangle> triangles;
        AABB bounds;
        const std::vector<Vertex> vertices;
        const std::vector<uint> indices;
    public:
    
        std::string name;

        Object(const std::vector<Vertex>& vertices, const std::vector<uint>& indices);
        ~Object();
        
        
        /// @brief Locks the frame and converts vertex data to triangles
        void GenerateTriangles();

        std::vector<Triangle> GetGeneratedTriangles() const;

        AABB GetBounds();

        /// @brief 
        /// @param ray 
        /// @return 
        bool Intersect(Ray& ray, Surface& hit);

        Frame frame;
        std::shared_ptr<Material> material;
    };

    std::vector<Object> LoadModel(const std::string& filepath);
    
} // namespace Tint
