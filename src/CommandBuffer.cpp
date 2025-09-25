#include "CommandBuffer.hpp"

#include "Buffer.hpp"

using namespace std;

namespace sqrp
{
	CommandBuffer::CommandBuffer(const Device& device, QueueContextType queueType)
		: pDevice_(&device), queueType_(queueType)
	{
		auto it = pDevice_->GetQueueContexts().find(queueType_);
		if (it == pDevice_->GetQueueContexts().end()) {
			throw std::runtime_error("No such queue context type");
		}
		const auto& context = it->second;
		//commandBuffer = CreateCommandBuffer(pDevice_->GetDevice(), context.commandPool.get());
		commandBuffer_ = std::move(pDevice_->GetDevice().allocateCommandBuffersUnique(
			vk::CommandBufferAllocateInfo()
			.setCommandPool(context.commandPool.get())
			.setCommandBufferCount(1)
			.setLevel(vk::CommandBufferLevel::ePrimary)
		).front());
	}

	void CommandBuffer::Begin(vk::CommandBufferUsageFlags flag)
	{
		commandBuffer_->begin(
			vk::CommandBufferBeginInfo()
			.setFlags(flag)
		);
	}

	void CommandBuffer::End()
	{
		return commandBuffer_->end();
	}

	void CommandBuffer::CopyBuffer(BufferHandle srcBuffer, BufferHandle dstBuffer)
	{
		commandBuffer_->copyBuffer(
			srcBuffer->GetBuffer(), dstBuffer->GetBuffer(),
			vk::BufferCopy()
			.setSize(min(srcBuffer->GetSize(), dstBuffer->GetSize()))
		);
	}

	void CommandBuffer::CopyBufferRegion(BufferHandle srcBuffer, vk::DeviceSize srcOffset, BufferHandle dstBuffer, vk::DeviceSize dstOffset, vk::DeviceSize size)
	{
		commandBuffer_->copyBuffer(
			srcBuffer->GetBuffer(), dstBuffer->GetBuffer(),
			vk::BufferCopy()
				.setSrcOffset(srcOffset)
				.setDstOffset(dstOffset)
				.setSize(size)
		);
	}

	vk::CommandBuffer CommandBuffer::GetCommandBuffer() const
	{
		return commandBuffer_.get();
	}
}