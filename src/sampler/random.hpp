#pragma once
#include <random>

namespace Tint
{
    struct RandomState
    {
        RandomState(uint32_t seed) : rng(seed) {};
        std::mt19937 rng;
    };

    class Random
    {
    public:
        static float Uniform1D(RandomState &state)
        {
            static std::uniform_real_distribution<float> distr(0, 1);
            return distr(state.rng);
        }

        static glm::vec2 Uniform2D(RandomState &state)
        {
            static std::uniform_real_distribution<float> distr(0, 1);
            return glm::vec2(distr(state.rng), distr(state.rng));
        }

        static glm::vec3 Uniform3D(RandomState &state)
        {
            static std::uniform_real_distribution<float> distr(0, 1);
            return glm::vec3(distr(state.rng), distr(state.rng), distr(state.rng));
        }

        static float UniformInRange(float min, float max, RandomState &state)
        {
            static std::uniform_real_distribution<float> distr(0, 1);
            return distr(state.rng) * (max - min) + min;
        }

        static glm::vec3 UniformHemisphere(RandomState& state, const glm::vec3& normal)
        {
            float u1 = Uniform1D(state);
            float u2 = Uniform1D(state);

            float z = u1;
            float r = glm::sqrt(glm::max(0.f, 1 - z * z));
            float phi = 2.0f * M_PI * u2;

            float x = r * glm::cos(phi);
            float y = r * glm::sin(phi);

            // Convert to world space
            glm::vec3 tangent, bitangent;
            CreateOrthonormalBasis(normal, tangent, bitangent);

            return x * tangent + y * bitangent + z * normal;
        }

        static glm::vec3 CosineHemisphere(RandomState& state, const glm::vec3& normal)
        {
            float u1 = Uniform1D(state);
            float u2 = Uniform1D(state);

            float r = glm::sqrt(u1);
            float phi = 2.0f * M_PI * u2;

            float x = r * glm::cos(phi);
            float y = r * glm::sin(phi);
            float z = glm::sqrt(glm::max(0.0f, 1.0f - u1));

            // Convert to world space
            glm::vec3 tangent, bitangent;
            CreateOrthonormalBasis(normal, tangent, bitangent);

            return x * tangent + y * bitangent + z * normal;
        }


    private:
        static void CreateOrthonormalBasis(const glm::vec3 &n, glm::vec3 &tangent, glm::vec3 &bitangent)
        {
            if (std::abs(n.z) < 1 - FLT_EPSILON)
            {
                tangent = glm::normalize(glm::cross(n, glm::vec3(0, 0, 1)));
            }
            else
            {
                tangent = glm::normalize(glm::cross(n, glm::vec3(0, 1, 0)));
            }
            bitangent = glm::normalize(glm::cross(tangent, n));
        }
    };
} // namespace Tint
