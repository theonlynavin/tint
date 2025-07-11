#pragma once
#include "object.hpp"
#include "camera.hpp"
#include "../intersection/bvh.hpp"
#include "../utils/texture.hpp"
#include <unordered_map>

namespace Tint
{
    class Scene
    {
    private:
        std::unordered_map<std::string, std::pair<TexturePtr, uint>> textures;
        std::unordered_map<std::string, std::pair<MaterialPtr, uint>> materaials;
        uint prevMaterialID, prevTextureID;
        
        std::vector<Object> objects;
        BVH* bvh;
    public:
        Scene();
        ~Scene();
        //void SetCamera(Camera* cam);
        void AddObject(const Object& object);
        void AddObjects(const std::vector<Object>& object);
        
        void BuildBVH();

        MaterialPtr FetchMaterial(const std::string &name);
        TexturePtr FetchTexture(const std::string &name);
        
        void PushMaterial(const Material& material, const std::string& name);
        void PushTexture(const Texture& texture, const std::string& name);
        
        std::pair<std::vector<gl::BVHNode>, std::vector<gl::Triangle>> BuildLBVH();
        
        bool ClosestIntersection(Ray& ray, Surface& surface);
    };
} // namespace Tint
