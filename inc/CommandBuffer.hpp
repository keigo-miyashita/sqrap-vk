#pragma once

#include "pch.hpp"

#include "Alias.hpp"

#include "Device.hpp"

namespace sqrp
{
	class Device;
	class FrameBuffer;
	class Image;
	class RenderPass;
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
		void BeginRenderPass(RenderPassHandle pRenderPass, FrameBufferHandle pFrameBuffer);
		void EndRenderPass();
		void CopyBuffer(BufferHandle srcBuffer, BufferHandle dstBuffer);
		void CopyBufferRegion(BufferHandle srcBuffer, vk::DeviceSize srcOffset, BufferHandle dstBuffer, vk::DeviceSize dstOffset, vk::DeviceSize size);
		void SetScissor(uint32_t width, uint32_t height);
		void SetViewport(uint32_t width, uint32_t height);
		void TransitionLayout(ImageHandle pImage, vk::ImageLayout newLayout);

		vk::CommandBuffer GetCommandBuffer() const;
	};
}