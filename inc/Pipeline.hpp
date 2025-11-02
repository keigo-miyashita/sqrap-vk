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
	protected:
		const Device* pDevice_ = nullptr;

		vk::UniquePipeline pipeline_;
		vk::UniquePipelineLayout pipelineLayout_;


	public:
		Pipeline(
			const Device& device
		);
		~Pipeline() = default;

		vk::Pipeline GetPipeline() const;
		vk::PipelineLayout GetPipelineLayout() const;
	};

	class GraphicsPipeline : public Pipeline
	{
	public:
		GraphicsPipeline(
			const Device& device,
			RenderPassHandle pRenderPass,
			SwapchainHandle pSwapchain,
			ShaderHandle pVertexShader,
			ShaderHandle pPixelShader,
			DescriptorSetHandle pDescriptorSet,
			vk::PushConstantRange pushConstantRange = vk::PushConstantRange{},
			bool enableDepthWrite = true,
			bool needVertexBuffer = true
		);
		~GraphicsPipeline() = default;
	};

	class ComputePipeline : public Pipeline
	{
	public:
		ComputePipeline(
			const Device& device,
			ShaderHandle pComputeShader,
			DescriptorSetHandle pDescriptorSet,
			vk::PushConstantRange pushConstantRange = vk::PushConstantRange{}
		);
		~ComputePipeline() = default;
	};
}