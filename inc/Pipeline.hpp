#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Device;
	class RenderPass;
	class Shader;
	class Swapchain;

	class Pipeline
	{
	private:
		const Device* pDevice_ = nullptr;

		vk::UniquePipeline pipeline_;


	public:
		Pipeline(
			const Device pDevice,
			RenderPassHandle pRenderPass,
			SwapchainHandle pSwapchain,
			ShaderHandle pVertexShader,
			ShaderHandle pPixelShader,
			vk::DescriptorSetLayout descriptorSetLayout
		);
		~Pipeline() = default;
	};
}