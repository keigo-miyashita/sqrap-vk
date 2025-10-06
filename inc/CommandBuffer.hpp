#pragma once

#include "pch.hpp"

#include "Alias.hpp"

#include "Device.hpp"

namespace sqrp
{
	class DescriptorSet;
	class Device;
	class FrameBuffer;
	class Image;
	class Mesh;
	class Pipeline;
	class RenderPass;
	class Swapchain;
	struct QueueContext;

	class CommandBuffer
	{
	private:
		const Device* pDevice_ = nullptr;
		QueueContextType queueType_ = QueueContextType::General;
		vk::UniqueCommandBuffer commandBuffer_;

	public:
		CommandBuffer(const Device& device, QueueContextType queueType = QueueContextType::General);
		~CommandBuffer() = default;

		void Begin();
		void Begin(vk::CommandBufferUsageFlags flag);
		void End();
		void BeginRender(SwapchainHandle pSwapchain);
		void EndRender(SwapchainHandle pSwapchain);
		void BeginRenderPass(RenderPassHandle pRenderPass, FrameBufferHandle pFrameBuffer);
		void BeginRenderPass(RenderPassHandle pRenderPass, FrameBufferHandle pFrameBuffer, SwapchainHandle pSwapchain);
		void EndRenderPass();
		void BindPipeline(PipelineHandle pPipeline, vk::PipelineBindPoint pipelineBindPoint);
		void BindMeshBuffer(MeshHandle pMesh);
		void BindDescriptorSet(PipelineHandle pPipeline, DescriptorSetHandle pDescriptorSet, vk::PipelineBindPoint pipelineBindPoint);
		void CopyBuffer(BufferHandle srcBuffer, BufferHandle dstBuffer);
		void CopyBufferRegion(BufferHandle srcBuffer, vk::DeviceSize srcOffset, BufferHandle dstBuffer, vk::DeviceSize dstOffset, vk::DeviceSize size);
		void SetScissor(uint32_t width, uint32_t height);
		void SetViewport(uint32_t width, uint32_t height);
		void TransitionLayout(ImageHandle pImage, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

		void DrawMesh(MeshHandle pMesh);
		void Draw(uint32_t vertexCount, uint32_t instanceCount);

		vk::CommandBuffer GetCommandBuffer() const;
	};
}