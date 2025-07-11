#pragma once
#include "frame.hpp"
#include "../intersection/aabb.hpp"
#include "../resources/mesh.hpp"

#include "material.hpp"
#include <vector>
#include <string>

namespace Tint
{
    struct Ray;
    class Material;
    class Mesh;

    typedef std::shared_ptr<Mesh> MeshPtr;
    typedef std::shared_ptr<Material> MaterialPtr;
    typedef std::shared_ptr<Texture> TexturePtr;

    typedef std::pair<MeshPtr, MaterialPtr> MeshMat;
    
    class Object
    {
    private:
        std::vector<Triangle> triangles;        
        std::vector<MeshMat> meshes;
        AABB bounds;
    public:
        Object(const std::vector<MeshMat>& meshes);
        ~Object();
        
        /// @brief Locks the frame and converts vertex data to triangles
        void GenerateTriangles();

        std::vector<Triangle> GetGeneratedTriangles() const;
        std::vector<MeshMat> GetRawMeshes() const;

        void AddMesh(const std::vector<MeshMat>& meshes);

        /// @brief Returns the axis-aligned bounding box for the union of all meshes
        /// @note GenerateTriangles() must have been called before
        AABB GetBounds() const;

        /// @brief Tests the ray against the meshes of the model
        /// @note GenerateTriangles() must have been called before
        /// @param ray Ray (ray distances may be modified)
        /// @param surf Surface of hit triangle (properties may be modified)
        /// @return True if the ray hits the object, False otherwise
        bool Intersect(Ray& ray, Surface& surf) const;

        Frame frame;    // Transformation frame
    };    
} // namespace Tint
