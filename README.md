# Sqrap for DirectX 12

**Sqrap** is a personal wrapper of low level graphics API, and this repository is for Vulkan.

## Requirements
- MSVC (cl.exe)
- CMake

## In progress
- Linux support
- Android support
- Ray tracing
- Mesh shader sample

## Standalone Build
```bash

# NOTE : Make sure you can use cl.exe from command line.

git clone --recurse-submodules https://github.com/keigo-miyashita/sqrap-vk.git

cd sqrap-vk
cmake --preset win-x64-Debug/Release
cmake --build --preset win-x64-Debug/Release

```
  
## Usage

```powershell
mkdir PROJECT_DIR
git init
git submodule add https://github.com/keigo-miyashita/sqrap-dx12.git
```