#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class DescriptorSet;
	class Device;
	class RenderPass;
	class Shader;
	class Swapchain;

	class Pipeline
	{
	private:
		const Device* pDevice_ = nullptr;

		vk::UniquePipeline pipeline_;
		vk::UniquePipelineLayout pipelineLayout_;


	public:
		Pipeline(
			const Device& device,
			RenderPassHandle pRenderPass,
			SwapchainHandle pSwapchain,
			ShaderHandle pVertexShader,
			ShaderHandle pPixelShader,
			DescriptorSetHandle pDescriptorSet
		);
		~Pipeline() = default;
	};
}