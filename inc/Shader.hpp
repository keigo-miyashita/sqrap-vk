#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Device;

	enum class ShaderType
	{
		Vertex, Pixel, Geometry, Domain, Hull, Amplification, Mesh, Compute, RayTracing, WorkGraph
	};

	inline int ToInt(ShaderType type)
	{
		return static_cast<int>(type);
	}

	class Shader
	{
	private:
		const Device* pDevice_ = nullptr;
		vk::UniqueShaderModule shaderModule_;

	public:
		Shader(const Device& device, const std::string& filename);
		~Shader() = default;
	};
}