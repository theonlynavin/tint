#include "bvh.hpp"
#include "aabb.hpp"
#include <algorithm>
#include <stack>

#define TINT_SAH_SPLITTING
//#define TINT_MID_SPLITTING

Tint::BVH::BVH(const std::vector<Triangle> &tris)
{
    if (tris.size() == 0)
        return;

    std::vector<BVHLeaf> bvhLeaves;
    for (size_t i = 0; i < tris.size(); i++)
    {
        bvhLeaves.push_back(BVHLeaf(i, AABB({tris[i]})));
    }

    root = Build(bvhLeaves, tris, 0, tris.size(), 0);
}

Tint::BVH::~BVH()
{
    if (root != nullptr)
    delete root;
}


int Tint::BVH::FlattenBVH(BVHNode *root, std::vector<gl_BVHNode> &nodes) const
{
    int offset = nodes.size();
    nodes.push_back(gl_BVHNode());

    gl_BVHNode& node = nodes[offset];
    node.aabb_max = root->bounds.GetMax();
    node.aabb_min = root->bounds.GetMin();
    
    if (root->numTris > 0)
    {
        node.is_leaf = true;
        node.tri_offset = root->offset;
        node.tri_count = root->numTris;
    }
    else
    {
        node.is_leaf = false;
        node.first_child = FlattenBVH(root->children[0], nodes);
        node.second_child = FlattenBVH(root->children[1], nodes);
    }

    return offset;
}

std::vector<Tint::gl_BVHNode> Tint::BVH::ToGLBVH() const
{
    struct StackEntry {
        BVHNode* node;
        int parentIndex;
        bool isFirst;
    };

    std::vector<gl_BVHNode> out;
    std::stack<StackEntry> stack;

    stack.push({ root, -1, false });

    while (!stack.empty()) {
        auto entry = stack.top(); stack.pop();
        
        BVHNode* node = entry.node;
        if (!node) continue;

        int index = out.size();
        out.push_back(gl_BVHNode{});
        gl_BVHNode& linear = out[index];
        linear.aabb_max = node->bounds.GetMax();
        linear.aabb_min = node->bounds.GetMin();
        
        if (entry.parentIndex != -1) {
            if (entry.isFirst)
                out[entry.parentIndex].first_child = index;
            else
                out[entry.parentIndex].second_child = index;
        }

        if (node->numTris > 0) {
            linear.is_leaf = true;
            linear.tri_offset = node->offset;
            linear.tri_count = node->numTris;
        } else {
            linear.is_leaf = false;
            stack.push({ node->children[1], index, false });
            stack.push({ node->children[0],  index, true  });
        }
    }

    return out;
}

Tint::BVHNode *Tint::BVH::GetRoot() const
{
    return root;
}

std::vector<Tint::Triangle> Tint::BVH::GetTriangles() const
{
    return orderedTriangles;
}

bool Tint::BVH::Traverse(Ray &ray, Surface &surf) const
{
    BVHNode *currentNode = root;

    std::vector<BVHNode *> toVisit;
    toVisit.reserve(64);

    float tbox;

    bool hitTriangle = false;
    bool hitBVH = false;

    while (true)
    {
        if (currentNode->bounds.Intersect(ray, tbox))
        {
            hitBVH = true;
            if (currentNode->numTris > 0)
            {
                for (size_t i = currentNode->offset; i < currentNode->offset + currentNode->numTris; i++)
                {
                    const Triangle &tri = orderedTriangles[i];
                    glm::vec2 uv;
                    if (tri.intersect(ray, uv))
                    {
                        hitTriangle = true;
                        surf.hit = tri;
                        surf.uv = uv;
                    }
                }

                if (toVisit.empty())
                {
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
                float t0, t1;
                bool hit0 = currentNode->children[0]->bounds.Intersect(ray, t0);
                bool hit1 = currentNode->children[1]->bounds.Intersect(ray, t1);

                if (hit0 && hit1)
                {
                    if (t0 < t1)
                    {
                        toVisit.emplace_back(currentNode->children[1]);
                        currentNode = currentNode->children[0];
                    }
                    else
                    {
                        toVisit.emplace_back(currentNode->children[0]);
                        currentNode = currentNode->children[1];
                    }
                }
                else if (hit0)
                {
                    currentNode = currentNode->children[0];
                }
                else if (hit1)
                {
                    currentNode = currentNode->children[1];
                }
                else
                {
                    if (!toVisit.empty())
                    {
                        currentNode = toVisit.back();
                        toVisit.pop_back();
                    }
                    else
                        return hitTriangle;
                }
            }
        }
        else
        {
            if (toVisit.empty())
            {
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

Tint::BVHNode *Tint::BVH::Build(std::vector<BVHLeaf> &leaves, const std::vector<Triangle> &tris, uint first, uint last, int depth)
{
    AABB bounds;
    for (size_t i = first; i < last; i++)
        bounds.Expand(leaves[i].bounds);

    uint numTris = last - first;

    if (numTris <= 4 || depth >= 16)
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
        int splitAxis;
        if (!ComputeSplitAxis(leaves, first, last, splitAxis, centroidBounds)) // unusal case, triangles packed close by
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
            int mid = PartitionBVHNode(leaves, first, last, splitAxis, centroidBounds);

            if (mid != -1)
                return BVHNode::CreateBranch(splitAxis,
                                         Build(leaves, tris, first, mid, depth+1),
                                         Build(leaves, tris, mid, last, depth+1),
                                         bounds);
            else
            {
                int offset = orderedTriangles.size();
                for (size_t i = first; i < last; i++)
                {
                    orderedTriangles.push_back(tris[leaves[i].triangleIndex]);
                }
                return BVHNode::CreateLeaf(offset, numTris, bounds);
            }
        }
    }

    return nullptr;
}

int Tint::BVH::PartitionBVHNode(std::vector<BVHLeaf> &leaves, uint first, uint last, int splitAxis, const AABB &centroidBounds)
{
    constexpr int maxTrisInLeaf = 4;
#ifdef TINT_MID_SPLITTING
    int mid = (first + last) / 2;
    std::nth_element(&leaves[first], &leaves[mid], &leaves[last - 1] + 1,

                     [splitAxis](const BVHLeaf &a, const BVHLeaf &b)
                     {
                         return a.centroid[splitAxis] < b.centroid[splitAxis];
                     });

    return mid;
#elif defined TINT_SAH_SPLITTING

    if (last - first < maxTrisInLeaf)
    {
        // split by middle
        int mid = (first + last) / 2;
        std::nth_element(&leaves[first], &leaves[mid], &leaves[last - 1] + 1,
                         [splitAxis](const BVHLeaf &a, const BVHLeaf &b)
                         {
                             return a.centroid[splitAxis] < b.centroid[splitAxis];
                         });
        return mid;
    }
    else
    {
        // binning triangles
        constexpr uint numBins = 14;

        struct Bins
        {
            uint numTris = 0;
            AABB bounds;
        };
        Bins bins[numBins];

        for (size_t i = first; i < last; i++)
        {
            int bin = numBins * 
                                (leaves[i].centroid[splitAxis] - centroidBounds.GetMin()[splitAxis]) 
                                / (centroidBounds.GetMax()[splitAxis] - centroidBounds.GetMin()[splitAxis]);
            if (bin == numBins)
                bin--;

            bins[bin].numTris++;
            bins[bin].bounds.Expand(leaves[i].bounds);
        }

        // computing SAH costs (alongside minCost)
        float costs[numBins - 1];
        float minCost = FLT_MAX;
        int splitBin = -1;
        for (size_t i = 0; i < numBins - 1; i++)
        {
            AABB first, second;
            int firstTris = 0, secondTris = 0;
            
            for (size_t j = 0; j < i+1; j++)
            {
                first.Expand(bins[j].bounds);
                firstTris += bins[j].numTris;
            }
            
            for (size_t j = i+1; j < numBins; j++)
            {
                second.Expand(bins[j].bounds);
                secondTris += bins[j].numTris;
            }

            costs[i] = 0.125f + (first.GetArea() * firstTris + second.GetArea() * secondTris) 
                                / AABB::Union(first, second).GetArea();

            if (costs[i] < minCost)
            {
                minCost = costs[i];
                splitBin = i;
            }
        }    

        float leafCost = last - first;
        if (!(minCost < leafCost || leafCost > maxTrisInLeaf))
            return -1;

        BVHLeaf* midPointer = std::partition(&leaves[first], &leaves[last-1]+1,
            [=](const BVHLeaf& l) 
            {
                int bin = numBins * (l.centroid[splitAxis] - centroidBounds.GetMin()[splitAxis]) 
                                    / (centroidBounds.GetMax()[splitAxis] - centroidBounds.GetMin()[splitAxis]);
                if (bin == numBins)
                    bin--;

                return bin <= splitBin;
            }
        );

        return midPointer - &leaves[0];
    }

#endif
}

bool Tint::BVH::ComputeSplitAxis(std::vector<BVHLeaf> &leaves, uint first, uint last, int &splitAxis, AABB &centroidBounds) const
{
    for (size_t i = first; i < last; i++)
    {
        centroidBounds.Expand(leaves[i].centroid);
    }

    splitAxis = 0; // x-axis

    glm::vec3 size = centroidBounds.GetMax() - centroidBounds.GetMin();
    if (size.y > size.x && size.y > size.z)
        splitAxis = 1;
    else if (size.z > size.x && size.z > size.y)
        splitAxis = 2;

    return (size != vec3(0, 0, 0));
}

Tint::BVHNode::~BVHNode()
{
    if (children[0] != nullptr)
        delete children[0];
    if (children[1] != nullptr)
        delete children[1];
}

Tint::BVHNode *Tint::BVHNode::CreateBranch(int splitAxis, BVHNode *first, BVHNode *second, const AABB &bounds)
{
    BVHNode *node = new BVHNode;
    node->bounds = bounds;
    node->children[0] = first;
    node->children[1] = second;
    node->splitAxis = splitAxis;
    node->numTris = 0;
    return node;
}

Tint::BVHNode *Tint::BVHNode::CreateLeaf(int offset, int numTris, const AABB &bounds)
{
    BVHNode *node = new BVHNode;
    node->bounds = bounds;
    node->offset = offset;
    node->children[0] = nullptr;
    node->children[1] = nullptr;
    node->numTris = numTris;
    return node;
}
