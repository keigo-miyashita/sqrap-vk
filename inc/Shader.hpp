#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Device;

	class Shader
	{
	private:
		const Device* pDevice_ = nullptr;

		vk::UniqueShaderModule shaderModule_;
		vk::ShaderStageFlagBits shaderStage_;

	public:
		Shader(const Device& device, const std::string& filename, vk::ShaderStageFlagBits shaderStage);
		~Shader() = default;

		vk::ShaderModule GetShaderModule() const;
		vk::ShaderStageFlagBits GetShaderStage() const;
	};
}