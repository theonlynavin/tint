#pragma once
#include "../utils/utils.hpp"
#include "triangle.hpp"
#include "aabb.hpp"

namespace Tint
{
    struct Surface;

    struct cl_BVHNode
    {   
        cl_float3 aabb_min;
        cl_float3 aabb_max;
    
        int first_child;
        int second_child;
        int tri_offset;
        int tri_count;
    
        int is_leaf;
        int pad0, pad1, pad2; // padding for alignment
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

        std::vector<cl_BVHNode> ToCLBVH() const;
        BVHNode* GetRoot() const;
        std::vector<Triangle> GetTriangles() const;
        bool Traverse(Ray& ray, Surface& surf) const;

    private:
        BVHNode* root;
        std::vector<Triangle> orderedTriangles;
        int FlattenBVH(BVHNode* root, std::vector<cl_BVHNode>& nodes) const;
        BVHNode* Build(std::vector<BVHLeaf>& leaves, const std::vector<Triangle>& tris, uint first, uint last, int depth);
        inline int PartitionBVHNode(std::vector<BVHLeaf>& leaves, uint first, uint last, int splitAxis, const AABB& centroidBounds);
        inline bool ComputeSplitAxis(std::vector<BVHLeaf>& leaves, uint first, uint last, int& splitAxis, AABB& centroidBounds) const;
    };    
} // namespace Tint
