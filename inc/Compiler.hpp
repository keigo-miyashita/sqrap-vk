#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	enum class ShaderType {
		Vertex, Pixel, Geometry, Domain, Hull, Amplification, Mesh, Compute, RayGen, Miss, ClosestHit, AnyHit, Intersection, Callable
	};

	class Shader;

	class Compiler
	{
	private:
		const Device* pDevice_ = nullptr;

		TBuiltInResource DefaultTBuiltInResource() const;

	public:
		Compiler();
		~Compiler();

		std::vector<uint32_t> CompileGLSLToSPIRV(const std::string& fileName, ShaderType shaderType) const;
	};
}