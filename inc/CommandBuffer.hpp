#pragma once

#include "pch.hpp"

#include "Alias.hpp"

#include "Device.hpp"

namespace sqrp
{
	class Device;
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

		void Begin(vk::CommandBufferUsageFlags flag);
		void End();
		void CopyBuffer(BufferHandle srcBuffer, BufferHandle dstBuffer);
		void CopyBufferRegion(BufferHandle srcBuffer, vk::DeviceSize srcOffset, BufferHandle dstBuffer, vk::DeviceSize dstOffset, vk::DeviceSize size);

		vk::CommandBuffer GetCommandBuffer() const;
	};
}