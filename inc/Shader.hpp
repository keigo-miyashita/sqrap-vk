#pragma once

#include "pch.hpp"

#include "Alias.hpp"

#include "Compiler.hpp"

namespace sqrp
{
	class Device;

	class Shader
	{
	private:
		const Device* pDevice_ = nullptr;
		const Compiler* pCompiler_ = nullptr;

		vk::UniqueShaderModule shaderModule_;
		vk::ShaderStageFlagBits shaderStage_;

	public:
		Shader(const Device& device, const Compiler& compiler, const std::string& fileName, ShaderType shaderType);
		~Shader() = default;

		vk::ShaderModule GetShaderModule() const;
		vk::ShaderStageFlagBits GetShaderStage() const;
	};
}