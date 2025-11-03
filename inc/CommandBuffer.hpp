#pragma once

#include "pch.hpp"

#include "Alias.hpp"

#include "Device.hpp"

namespace sqrp
{
	class DescriptorSet;
	class Device;
	class FrameBuffer;
	class GUI;
	class Image;
	class MeshBase;
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
		void BeginRenderPass(RenderPassHandle pRenderPass, FrameBufferHandle pFrameBuffer, uint32_t imageIndex);
		void EndRenderPass();
		void BindPipeline(PipelineHandle pPipeline, vk::PipelineBindPoint pipelineBindPoint);
		void BindMeshBuffer(MeshBaseHandle pMesh);
		void BindMeshBuffer(MeshBaseHandle pMesh, int vertexByteOffset, int indexByteOffset);
		void BindDescriptorSet(PipelineHandle pPipeline, DescriptorSetHandle pDescriptorSet, vk::PipelineBindPoint pipelineBindPoint);
		void PushConstants(PipelineHandle pPipeline, vk::ShaderStageFlags stageFlags, uint32_t size, const void* pValues);
		void CopyBuffer(BufferHandle srcBuffer, BufferHandle dstBuffer);
		void CopyBufferRegion(BufferHandle srcBuffer, vk::DeviceSize srcOffset, BufferHandle dstBuffer, vk::DeviceSize dstOffset, vk::DeviceSize size);
		void CopyBufferToImage(BufferHandle srcBuffer, ImageHandle dstImage);
		void SetScissor(uint32_t width, uint32_t height);
		void SetViewport(uint32_t width, uint32_t height);
		void TransitionLayout(ImageHandle pImage, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
		void ImageBarrier(
			ImageHandle pImage,
			vk::ImageLayout oldLayout,
			vk::ImageLayout newLayout,
			vk::PipelineStageFlags srcStageMask = vk::PipelineStageFlagBits::eAllCommands,
			vk::PipelineStageFlags dstStageMask = vk::PipelineStageFlagBits::eAllCommands,
			vk::AccessFlags srcAccessMask = {},
			vk::AccessFlags dstAccessMask = {}
		);

		void DrawMesh(MeshBaseHandle pMesh, int numIndices);
		void Draw(uint32_t vertexCount, uint32_t instanceCount);
		void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

		void DrawGui(GUI& gui);

		vk::CommandBuffer GetCommandBuffer() const;
	};
}