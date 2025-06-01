#pragma once
#include <random>

namespace Tint
{
    struct RandomState
    {
        RandomState(uint32_t seed): rng(seed) {};
        std::mt19937 rng;
    };

    class Random
    {
    public:
        static float Uniform1D(RandomState& state) 
        {
            static std::uniform_real_distribution<float> distr(0, 1);
            return distr(state.rng);
        }

        static glm::vec2 Uniform2D(RandomState& state) 
        {
            static std::uniform_real_distribution<float> distr(0, 1);
            return glm::vec2(distr(state.rng), distr(state.rng));
        }

        static glm::vec3 Uniform3D(RandomState& state) 
        {
            static std::uniform_real_distribution<float> distr(0, 1);
            return glm::vec3(distr(state.rng), distr(state.rng), distr(state.rng));
        }

        static float UniformInRange(float min, float max, RandomState& state) 
        {
            static std::uniform_real_distribution<float> distr(0, 1);
            return distr(state.rng) * (max - min) + min;
        }
    };
} // namespace Tint
