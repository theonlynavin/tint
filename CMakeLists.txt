cmake_minimum_required(VERSION 3.10)

project(tint VERSION 1.0)
set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED TRUE)

# Required 3rd Party Libraries 
find_package(glm REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

# Project specific configuration
add_executable(tint 
    src/main.cpp
    src/intersection/aabb.cpp
    src/intersection/bvh.cpp
    src/intersection/triangle.cpp
    src/resources/mesh.cpp
    src/resources/image.cpp
    src/resources/resources.cpp
    src/scene/camera.cpp
    src/scene/object.cpp
    src/scene/material.cpp
    src/scene/scene.cpp
    src/scene/frame.cpp
    src/utils/buffer.cpp
    src/utils/shader.cpp
    src/utils/texture.cpp
    src/utils/utils.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
     ${CMAKE_BINARY_DIR}/../bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)

target_include_directories(tint PRIVATE include(glm::glm))
target_include_directories(tint PRIVATE GLFW_INCLUDE_DIRS)
target_include_directories(tint PRIVATE GLEW_INCLUDE_DIRS)
target_include_directories(tint PRIVATE ${CMAKE_BINARY_DIR}/../external/)

target_link_libraries(tint PRIVATE glfw)
target_link_libraries(tint PRIVATE GLEW::GLEW)
target_link_libraries(tint PRIVATE OpenGL::GL)
target_link_libraries(tint PRIVATE OpenGL::OpenGL)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")

    target_compile_definitions(${PROJECT_NAME} PRIVATE TINT_DEBUG)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -O2)
endif()