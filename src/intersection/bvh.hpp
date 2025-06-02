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

            static BVHNode *CreateBranch(int splitAxis, BVHNode* first, BVHNode* second);
            static BVHNode *CreateLeaf(int offset, int numTris, const AABB& bounds);

            BVHNode* children[2];
            AABB bounds;
            int splitAxis;
            int numTris;
            int offset;
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
        
        
        BVH(const std::vector<Triangle>& tris, int maxTrisInNode);
        ~BVH();

        BVHNode* root;

        bool Traverse(const Ray& ray, Surface& surf, float& tmin);

    private:
        std::vector<Triangle> orderedTriangles;
        BVHNode* RecursiveBuild(std::vector<BVHLeaf>& leaves, const std::vector<Triangle>& tris, uint first, uint last, uint maxTrisInLeaf);
    };
    
} // namespace Tint
