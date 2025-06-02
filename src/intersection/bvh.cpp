#include "bvh.hpp"
#include "aabb.hpp"
#include <algorithm>

Tint::BVH::BVH(const std::vector<Triangle>& tris, int maxTrisInLeaf)
{
    if (tris.size() == 0)
        return;

    std::vector<BVHLeaf> bvhLeaves;
    for (size_t i = 0; i < tris.size(); i++)
    {
        bvhLeaves.push_back(BVHLeaf(i, AABB({tris[i]})));
    }

    root = RecursiveBuild(bvhLeaves, tris, 0, tris.size(), maxTrisInLeaf);
}

Tint::BVH::~BVH()
{
    delete root;
}

bool Tint::BVH::Traverse(const Ray &ray, Surface &surf, float& tmin)
{
    BVHNode* currentNode = root;

    std::vector<BVHNode*> toVisit = {};

    float tbox;
    bool hitTriangle = false;
    float minDist = FLT_MAX;

    while (true)
    {
        if (currentNode->bounds.Intersect(ray, tbox))
        {
            if (currentNode->numTris > 0)
            {
                for (size_t i = currentNode->offset; i < currentNode->offset + currentNode->numTris; i++)
                {
                    Triangle& tri = orderedTriangles[i];
            
                    float t = 0;
                    glm::vec2 uv;
                    if (tri.intersect(ray, uv, t))
                    {
                        hitTriangle = true;
                        if (t < minDist)
                        {
                            minDist = t;
                            surf.hit = tri;
                            surf.uv = uv;
                        }
                    }
                }

                if (toVisit.size() == 0)
                {
                    tmin = minDist;
                    return hitTriangle;
                }
                else
                {
                    currentNode = toVisit.back();
                    toVisit.pop_back();
                }
            }
            else
            {
                if (currentNode->children[0]->bounds.Intersect(ray, tbox))
                    toVisit.push_back(currentNode->children[0]);
                if (currentNode->children[1]->bounds.Intersect(ray, tbox))
                    toVisit.push_back(currentNode->children[1]);

                if (toVisit.size() != 0)
                {
                    currentNode = toVisit.back();
                    toVisit.pop_back();
                }
                else
                {
                    tmin = minDist;
                    return hitTriangle;
                }
            }
        }
        else
        {
            if (toVisit.size() == 0)
            {
                tmin = minDist;
                return hitTriangle;
            }
            else
            {
                if (toVisit.size() != 0)
                {
                    currentNode = toVisit.back();
                    toVisit.pop_back();
                }
            }
        }
    }
}

Tint::BVH::BVHNode *Tint::BVH::RecursiveBuild(std::vector<BVHLeaf>& leaves, const std::vector<Triangle>& tris, uint first, uint last, uint maxTrisInLeaf)
{
    AABB bounds;
    for (size_t i = first; i < last; i++)
    {
        bounds = AABB::Union(bounds, leaves[i].bounds);
    }

    uint numTris = last - first;

    if (numTris == 1)
    {
        int offset = orderedTriangles.size();
        for (size_t i = first; i < last; i++)
        {
            orderedTriangles.push_back(tris[leaves[i].triangleIndex]);
        }
    
        return BVHNode::CreateLeaf(offset, numTris, bounds);
    }
    else
    {
        AABB centroidBounds;
        for (size_t i = first; i < last; i++)
        {
            centroidBounds.Expand(leaves[i].centroid);
        }
        
        int splitAxis = 0;  // x-axis

        glm::vec3 size = centroidBounds.GetMax() - centroidBounds.GetMin();
        if (size.y > size.x && size.y > size.z)
            splitAxis = 1;
        else if (size.z > size.x && size.z > size.y)
            splitAxis = 2;

        int mid = (first + last) / 2;

        if (centroidBounds.GetMax() == centroidBounds.GetMin()) // unusal case, triangles packed close by
        {
            int offset = orderedTriangles.size();
            for (size_t i = first; i < last; i++)
            {
                orderedTriangles.push_back(tris[leaves[i].triangleIndex]);
            }
        
            return BVHNode::CreateLeaf(offset, numTris, bounds);
        }
        else
        {
            // split by middle
            std::nth_element(&leaves[first], &leaves[mid], &leaves[last-1]+1, 
                
                [splitAxis](const BVHLeaf &a, const BVHLeaf &b) { 
                    return a.centroid[splitAxis] < b.centroid[splitAxis];
                }
            );

            return BVHNode::CreateBranch(splitAxis, 
                RecursiveBuild(leaves, tris, first, mid, maxTrisInLeaf),
                RecursiveBuild(leaves, tris, mid, last, maxTrisInLeaf));
        }
    }

    return nullptr;
}

Tint::BVH::BVHNode::~BVHNode()
{
    if (children[0] != nullptr)
        delete children[0];
    if (children[1] != nullptr)
        delete children[1];
}

Tint::BVH::BVHNode *Tint::BVH::BVHNode::CreateBranch(int splitAxis, BVHNode *first, BVHNode *second)
{
    BVHNode* node = new BVHNode;
    node->bounds = AABB::Union(first->bounds, second->bounds);
    node->children[0] = first;
    node->children[1] = second;
    node->splitAxis = splitAxis;
    node->numTris = 0;
    return node;
}

Tint::BVH::BVHNode *Tint::BVH::BVHNode::CreateLeaf(int offset, int numTris, const AABB &bounds)
{
    BVHNode* node = new BVHNode;
    node->bounds = bounds;
    node->offset = offset;
    node->children[0] = nullptr;
    node->children[1] = nullptr;
    node->numTris = numTris;
    return node;
}
