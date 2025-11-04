add_executable(${PROJECT_NAME}-sample-raster)

target_compile_features(${PROJECT_NAME}-sample-raster PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME}-sample-raster PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus /utf-8>)
target_sources(${PROJECT_NAME}-sample-raster PRIVATE main.cpp "SampleApp.cpp")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# link sqrap-vk
target_link_libraries(
    ${PROJECT_NAME}-sample-raster PRIVATE
    sqrap-vk
)

# sqrap-vk include directory
target_include_directories(${PROJECT_NAME}-sample-raster PRIVATE "${PROJECT_SOURCE_DIR}/inc")

set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders/")
add_compile_definitions(SHADER_DIR="${SHADER_DIR}")
set(MODEL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../model/")
add_compile_definitions(MODEL_DIR="${MODEL_DIR}")