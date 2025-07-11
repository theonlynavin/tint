#pragma once
#include "../utils/utils.hpp"
#include "triangle.hpp"
#include "aabb.hpp"

namespace Tint
{
    struct Surface;

    namespace gl 
    {
        struct BVHNode
        {   
            glm::vec4 aabb_min; // .w = is_leaf
            glm::vec4 aabb_max; // .w is unused
            glm::vec4 data;     // .xy = first_child, second_child or tri_offset, tri_count
                                //       for is_leaf = 0              for is_leaf = 1
        };
    }

    class BVH
    {
    public:
        struct BVHNode
        {
            ~BVHNode();

            static BVHNode *CreateBranch(int splitAxis, BVHNode* first, BVHNode* second, const AABB& bounds);
            static BVHNode *CreateLeaf(int offset, int numTris, const AABB& bounds);

            BVHNode* children[2];
            AABB bounds;
            int splitAxis;
            int numTris;
            int offset = -1;
        };

        struct BVHLeaf
        {
            BVHLeaf(uint triangleIndex, const AABB& bounds)
                : triangleIndex(triangleIndex), bounds(bounds), centroid(bounds.GetCentroid())
            {
            }

            uint triangleIndex;
            AABB bounds;
            glm::vec3 centroid;
        };
        
        /// @brief Constructs a BVH given a set of triangles
        /// @param tris Triangles
        BVH(const std::vector<Triangle>& tris);

        /// @brief Deletes the constructed BVH
        ~BVH();

        std::vector<gl::BVHNode> ToGLBVH() const;

        /// @brief Returns the root node in the BVH-tree
        BVHNode* GetRoot() const;

        /// @brief Returns the triangles (in the order post BVH construction)
        std::vector<Triangle> GetTriangles() const;

        /// @brief Traverses the BVH looking for ray-triangle intersections
        /// @param ray Ray (ray distances may be modified)
        /// @param surf Surface of hit triangle (properties may be modified)
        /// @return True if hits a triangle, False otherwise 
        bool Traverse(Ray& ray, Surface& surf) const;

    private:
        BVHNode* root;
        std::vector<Triangle> orderedTriangles;
        BVHNode* Build(std::vector<BVHLeaf>& leaves, const std::vector<Triangle>& tris, uint first, uint last, int depth);
        inline int PartitionBVHNode(std::vector<BVHLeaf>& leaves, uint first, uint last, int splitAxis, const AABB& centroidBounds);
        inline bool ComputeSplitAxis(std::vector<BVHLeaf>& leaves, uint first, uint last, int& splitAxis, AABB& centroidBounds) const;
    };    
} // namespace Tint
