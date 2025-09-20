add_executable(${PROJECT_NAME}-sample-raster)

target_compile_features(${PROJECT_NAME}-sample-raster PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME}-sample-raster PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus /utf-8>)
target_sources(${PROJECT_NAME}-sample-raster PRIVATE main.cpp "SampleApp.cpp")

# ライブラリのリンク
target_link_libraries(
    ${PROJECT_NAME}-sample-raster PRIVATE
    sqrap-vk
)

# インクルードディレクトリへのパスを追加
# 作成したラッパーのincludeを指定
target_include_directories(${PROJECT_NAME}-sample-raster PRIVATE "${PROJECT_SOURCE_DIR}/inc")