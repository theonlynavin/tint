#pragma once
#include "../utils/utils.hpp"
#include "triangle.hpp"
#include "aabb.hpp"

namespace Tint
{
    struct Surface;

    struct gl_BVHNode
    {   
        glm::vec4 aabb_min; // .w = is_leaf
        glm::vec4 aabb_max; // .w is unused
        glm::vec4 data;     // .xy = first_child, second_child or tri_offset, tri_count
                            //       for is_leaf = 0              for is_leaf = 1
    };

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

    class BVH
    {
    public:

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
        
        
        BVH(const std::vector<Triangle>& tris);
        ~BVH();

        std::vector<gl_BVHNode> ToGLBVH() const;
        BVHNode* GetRoot() const;
        std::vector<Triangle> GetTriangles() const;
        bool Traverse(Ray& ray, Surface& surf) const;

    private:
        BVHNode* root;
        std::vector<Triangle> orderedTriangles;
        int FlattenBVH(BVHNode* root, std::vector<gl_BVHNode>& nodes) const;
        BVHNode* Build(std::vector<BVHLeaf>& leaves, const std::vector<Triangle>& tris, uint first, uint last, int depth);
        inline int PartitionBVHNode(std::vector<BVHLeaf>& leaves, uint first, uint last, int splitAxis, const AABB& centroidBounds);
        inline bool ComputeSplitAxis(std::vector<BVHLeaf>& leaves, uint first, uint last, int& splitAxis, AABB& centroidBounds) const;
    };    
} // namespace Tint
