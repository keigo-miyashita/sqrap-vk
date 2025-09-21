#include "CommandBuffer.hpp"

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
}