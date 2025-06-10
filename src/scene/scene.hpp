#pragma once
#include "object.hpp"
#include "../camera/camera.hpp"
#include "../intersection/bvh.hpp"

namespace Tint
{
    class Scene
    {
    private:
        //Camera* camera;
        std::vector<Object> objects;
        BVH* bvh;
    public:
        Scene();
        ~Scene();
        //void SetCamera(Camera* cam);
        void AddObject(const Object& object);
        void AddObjects(const std::vector<Object>& object);
        void BuildBVH();
        std::pair<std::vector<cl_BVHNode>, std::vector<cl_Triangle>> BuildLBVH();
        bool ClosestIntersection(Ray& ray, Surface& surface);
    };
} // namespace Tint
