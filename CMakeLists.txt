cmake_minimum_required(VERSION 3.10)

project(tint VERSION 1.0)
set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED TRUE)

# Required 3rd Party Libraries 
find_package(glm REQUIRED)

# Project specific configuration
add_executable(tint 
    src/main.cpp
    src/camera/camera.cpp
    src/intersection/aabb.cpp
    src/intersection/bvh.cpp
    src/intersection/object.cpp
    src/intersection/triangle.cpp
    src/utils/frame.cpp
    src/utils/image.cpp
    )

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
     ${CMAKE_BINARY_DIR}/../bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE TINT_DEBUG)

target_include_directories(tint PRIVATE include(glm::glm))
target_include_directories(tint PRIVATE ${CMAKE_BINARY_DIR}/../external/)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -O2)
endif()