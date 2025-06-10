#include "tint.hpp"
#include <chrono>

#define WIDTH 1600
#define HEIGHT 1200
#define spp 20

glm::vec3 TestColoring(Tint::Ray& r, Tint::Scene& scene, Tint::RandomState& state, int depth, int maxDepth = 4)
{
    if (depth == maxDepth)
        return glm::vec3(0);

    Tint::Surface surf;
    if (scene.ClosestIntersection(r, surf))
    {
        glm::vec3 n = surf.hit.normal(surf.uv);
        glm::vec3 orig = surf.hit.point(surf.uv) + n * 1e-3f;
        glm::vec3 dir = Tint::Random::CosineHemisphere(state, n);
        Tint::Ray r = Tint::Ray(orig, dir);

        return glm::vec3(0.5) * TestColoring(r, scene, state, depth++, maxDepth);
    }
    else
        return glm::vec3(0.8, 0.8, 0.9);
}

int main_cxx(int argc, char const *argv[])
{
    Tint::Image img(WIDTH, HEIGHT);

    Tint::Camera cam(glm::vec2(WIDTH, HEIGHT), M_PI_2, 2, 0);
    cam.LookAt(glm::vec3(-2, 2, 5), glm::vec3(0, 0, 0));
    cam.frame.LockTransform();

    Tint::Scene scene;
    std::vector<Tint::Object> model = Tint::LoadModel("assets/dragon.obj");
    //scene.AddObjects(model);
    model = Tint::LoadModel("assets/scene.obj");
    //model[0].frame.Scale(glm::vec3(2));
    //model[0].frame.Translate(glm::vec3(1, -1, 2));
    scene.AddObjects(model);

    auto begin = std::chrono::high_resolution_clock::now();

    scene.BuildBVH();
    
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms to build BVH!\n";

    begin = std::chrono::high_resolution_clock::now();

    Tint::RandomState state(0);
    for (size_t i = 0; i < WIDTH; i++)
    {
        for (size_t j = 0; j < HEIGHT; j++)
        {
            float u = (float(i) / WIDTH);
            float v = 1 - (float(j) / HEIGHT);

            glm::vec3 accum = glm::vec3(0);
            for (size_t s = 0; s < spp; s++)
            {
                Tint::Ray prim = cam.GenerateRay(u, v, state);
                accum += TestColoring(prim, scene, state, 0, 8);
            }

            img.SetPixel(i, j, glm::pow(accum * (1.f/spp), glm::vec3(1/2.2f)));

        }
    }

    end = std::chrono::high_resolution_clock::now();

    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms for rendering!\n";

    img.SaveAs("./test.jpg");

    return 0;
}

#include <fstream>
#define CL_CHECK(err) \
    if (err != CL_SUCCESS) { \
        std::cerr << "OpenCL Error at " << __FILE__ << ":" << __LINE__ << " - " \
                  << getCLErrorString(err) << " (" << err << ")" << std::endl; \
        std::exit(1); \
    }

std::string getCLErrorString(cl_int err) {
    switch (err) {
        case CL_SUCCESS: return "CL_SUCCESS";
        case CL_DEVICE_NOT_FOUND: return "CL_DEVICE_NOT_FOUND";
        case CL_DEVICE_NOT_AVAILABLE: return "CL_DEVICE_NOT_AVAILABLE";
        case CL_COMPILER_NOT_AVAILABLE: return "CL_COMPILER_NOT_AVAILABLE";
        case CL_MEM_OBJECT_ALLOCATION_FAILURE: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        case CL_OUT_OF_RESOURCES: return "CL_OUT_OF_RESOURCES";
        case CL_OUT_OF_HOST_MEMORY: return "CL_OUT_OF_HOST_MEMORY";
        case CL_BUILD_PROGRAM_FAILURE: return "CL_BUILD_PROGRAM_FAILURE";
        case CL_INVALID_VALUE: return "CL_INVALID_VALUE";
        case CL_INVALID_DEVICE: return "CL_INVALID_DEVICE";
        case CL_INVALID_BINARY: return "CL_INVALID_BINARY";
        case CL_INVALID_KERNEL_NAME: return "CL_INVALID_KERNEL_NAME";
        case CL_INVALID_KERNEL_ARGS: return "CL_INVALID_KERNEL_ARGS";
        default: return "Unknown Error";
    }
}

// Load OpenCL source code from file (your kernel file)
std::string loadKernelSource(const char* filename) {
    std::ifstream file(filename);
    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

// opencl main
int main(int argc, char const *argv[])
{
    Tint::Image img(WIDTH, HEIGHT);

    Tint::Camera cam(glm::vec2(WIDTH, HEIGHT), M_PI_2, 2, 0);
    cam.LookAt(glm::vec3(-2, 2, 5), glm::vec3(0, 0, 0));
    cam.frame.LockTransform();

    auto cl_camera = cam.ToCLCamera();

    Tint::Scene scene;
    std::vector<Tint::Object> model = Tint::LoadModel("assets/dragon.obj");
    scene.AddObjects(model);
    model = Tint::LoadModel("assets/bunny.obj");
    model[0].frame.Scale(glm::vec3(2));
    model[0].frame.Translate(glm::vec3(1, -1, 2));
    scene.AddObjects(model);

    auto begin = std::chrono::high_resolution_clock::now();

    auto cl_data = scene.BuildLBVH();

    auto end = std::chrono::high_resolution_clock::now();

    std::vector<cl_float3> aabb_max,aabb_min;
    std::vector<int> first_child, second_child, tri_count, tri_offset, is_leaf;
    for (size_t i = 0; i < cl_data.second.size(); i++)
    {
        aabb_max.push_back(cl_data.first[i].aabb_max);
        aabb_min.push_back(cl_data.first[i].aabb_min);
        first_child.push_back(cl_data.first[i].first_child);
        second_child.push_back(cl_data.first[i].second_child);
        tri_count.push_back(cl_data.first[i].tri_count);
        tri_offset.push_back(cl_data.first[i].tri_offset);
        is_leaf.push_back(cl_data.first[i].is_leaf);
    }

    std::vector<cl_float3> v0,v1,v2,n0,n1,n2;
    for (size_t i = 0; i < cl_data.second.size(); i++)
    {
        v0.push_back(cl_data.second[i].v0);
        v1.push_back(cl_data.second[i].v1);
        v2.push_back(cl_data.second[i].v2);
        n0.push_back(cl_data.second[i].n0);
        n1.push_back(cl_data.second[i].n1);
        n2.push_back(cl_data.second[i].n2);
    }
    

    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms to build BVH!\n";
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty()) throw std::runtime_error("No OpenCL platforms found.");
    cl::Platform platform = platforms.front();

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    if (devices.empty()) throw std::runtime_error("No OpenCL GPU devices found.");
    cl::Device device = devices.front();

    // 2. Create context and command queue
    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    // 3. Load & build kernel
    std::string build_options = "-I./src/opencl/ -cl-fast-relaxed-math -cl-mad-enable";
    std::string source = loadKernelSource("src/opencl/intersection.cl");

    cl::Program program(context, source);
    cl_int err = program.build({device}, build_options.c_str());
    if (err != CL_SUCCESS) {
        std::string build_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
        std::cerr << "Build failed:\n" << build_log << std::endl;
        CL_CHECK(err); // this will also exit
    }

    cl::Kernel kernel(program, "intersect_scene", &err);
    CL_CHECK(err);

    cl::Buffer aabb_minBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_float3) * aabb_min.size(), aabb_min.data(), &err);
    CL_CHECK(err);

    cl::Buffer aabb_maxBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_float3) * aabb_max.size(), aabb_max.data(), &err);
    CL_CHECK(err);

    cl::Buffer first_childBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(int) *  first_child.size(),  first_child.data(), &err);
    CL_CHECK(err);

    cl::Buffer second_childBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(int) *  second_child.size(),  second_child.data(), &err);
    CL_CHECK(err);

    cl::Buffer tri_offsetBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(int) *  tri_offset.size(),  tri_offset.data(), &err);
    CL_CHECK(err);
    cl::Buffer tri_countBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(int) *  tri_count.size(),  tri_count.data(), &err);
    CL_CHECK(err);
    cl::Buffer is_leafBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(int) *   is_leaf.size(),   is_leaf.data(), &err);
    CL_CHECK(err);

    cl::Buffer v0Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_float3) * v0.size(), v0.data(), &err);
    CL_CHECK(err);

    cl::Buffer v1Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_float3) * v1.size(), v1.data(), &err);
    CL_CHECK(err);

    cl::Buffer v2Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_float3) * v2.size(), v2.data(), &err);
    CL_CHECK(err);
    
    cl::Buffer n0Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_float3) * n0.size(), n0.data(), &err);
    CL_CHECK(err);

    cl::Buffer n1Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_float3) * n1.size(), n1.data(), &err);
    CL_CHECK(err);

    cl::Buffer n2Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(cl_float3) * n2.size(), n2.data(), &err);
    CL_CHECK(err);

    cl::Buffer depthBuffer(context, CL_MEM_WRITE_ONLY,
        sizeof(float) * WIDTH * HEIGHT, nullptr, &err);
    CL_CHECK(err);

    kernel.setArg(0, cl_camera);   // assuming Camera is trivially copyable
    kernel.setArg(1, aabb_minBuffer);
    kernel.setArg(2, aabb_maxBuffer);
    kernel.setArg(3, first_childBuffer);
    kernel.setArg(4, second_childBuffer);
    kernel.setArg(5, tri_offsetBuffer);
    kernel.setArg(6, tri_countBuffer);
    kernel.setArg(7, is_leafBuffer);
    kernel.setArg(8, v0Buffer);
    kernel.setArg(9, v1Buffer);
    kernel.setArg(10, v2Buffer);
    kernel.setArg(11, n0Buffer);
    kernel.setArg(12, n1Buffer);
    kernel.setArg(13, n2Buffer);
    kernel.setArg(14, n2.size());
    kernel.setArg(15, WIDTH);
    kernel.setArg(16, HEIGHT);
    kernel.setArg(17, depthBuffer);

    end = std::chrono::high_resolution_clock::now();
    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms for setting up OpenCL Buffers!\n";

    begin = std::chrono::high_resolution_clock::now();

    cl::NDRange global(WIDTH, HEIGHT);
    cl::NDRange local(16, 16);
    err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, global, local);
    CL_CHECK(err);

    std::vector<float> depth_data(WIDTH * HEIGHT);
    err = queue.enqueueReadBuffer(depthBuffer, CL_TRUE, 0,
        sizeof(float) * depth_data.size(), depth_data.data());
    CL_CHECK(err);
    end = std::chrono::high_resolution_clock::now();

    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms to render!\n";
    
    for (size_t i = 0; i < WIDTH; i++)
    {
        for (size_t j = 0; j < HEIGHT; j++)
        {
            //std::cout << depth_data[j * WIDTH + i] << std::endl;
            img.SetPixel(i, j, glm::vec3(depth_data[j * WIDTH + i]));
        }
    }

    img.SaveAs("./test.jpg");

    return 0;
}
