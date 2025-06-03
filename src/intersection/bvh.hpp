#include "../utils/utils.hpp"
#include "triangle.hpp"
#include "aabb.hpp"

namespace Tint
{
    struct Surface;

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
        
        
        BVH(const std::vector<Triangle>& tris);
        ~BVH();

        BVHNode* root;

        bool Traverse(Ray& ray, Surface& surf);

    private:
        std::vector<Triangle> orderedTriangles;
        BVHNode* Build(std::vector<BVHLeaf>& leaves, const std::vector<Triangle>& tris, uint first, uint last);
        inline int PartitionBVHNode(std::vector<BVHLeaf>& leaves, uint first, uint last, int splitAxis, const AABB& centroidBounds);
        inline bool ComputeSplitAxis(std::vector<BVHLeaf>& leaves, uint first, uint last, int& splitAxis, AABB& centroidBounds) const;
    };
    
} // namespace Tint
