cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(sqrap-vk CXX)

file(TO_CMAKE_PATH $ENV{VULKAN_SDK} VULKAN_SDK)
set(VULKAN_LIB ${VULKAN_SDK}/Lib)
set(VULKAN_INC ${VULKAN_SDK}/Include)
set(VULKAN_SRC ${VULKAN_SDK}/Source)

# 変数にファイルを格納
file(GLOB_RECURSE WRAPPER_SOURCES "src/*.cpp")
file(GLOB_RECURSE WRAPPER_HEADERS "inc/*.hpp")

# hpp, cppファイルのまま扱うためのターゲット設定
add_library(${PROJECT_NAME} ${WRAPPER_SOURCES})

find_package(glm CONFIG REQUIRED)
find_package(glfw3       REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
#find_package(VulkanHeaders CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)


# ヘッダーファイルのあるディレクトリをインクルード
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${TINYGLTF_INCLUDE_DIRS}
        ${VULKAN_INCLUDE}/
        ${VULKAN_SOURCE}/
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        glm::glm
        glfw
        imgui::imgui
        #Vulkan::Headers
        GPUOpen::VulkanMemoryAllocator
        glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV glslang::SPVRemapper
)

add_subdirectory(sample/raster)