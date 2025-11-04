cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(sqrap-vk CXX)

file(TO_CMAKE_PATH $ENV{VULKAN_SDK} VULKAN_SDK)
set(VULKAN_LIB ${VULKAN_SDK}/Lib)
set(VULKAN_INC ${VULKAN_SDK}/Include)
set(VULKAN_SRC ${VULKAN_SDK}/Source)

file(GLOB_RECURSE WRAPPER_SOURCES "src/*.cpp")
file(GLOB_RECURSE WRAPPER_HEADERS "inc/*.hpp")

add_library(${PROJECT_NAME} ${WRAPPER_SOURCES})

target_precompile_headers(${PROJECT_NAME} PUBLIC inc/pch.hpp)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)
find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")


# include header files directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${TINYGLTF_INCLUDE_DIRS}
        ${VULKAN_INC}/
        ${VULKAN_SOURCE}/
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        glfw
        glm::glm
        imgui::imgui
        GPUOpen::VulkanMemoryAllocator
        glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV glslang::SPVRemapper
        ${VULKAN_LIB}/vulkan-1.lib
)

add_subdirectory(sample/raster)