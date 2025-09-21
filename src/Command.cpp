#include "Command.hpp"

namespace sqrp
{
	vk::UniqueCommandBuffer CreateCommandBuffer(vk::Device dev, vk::CommandPool commandPool)
	{
		vk::CommandBufferAllocateInfo allocInfo{};
		allocInfo.setCommandPool(commandPool);
		allocInfo.setCommandBufferCount(1);
		allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);

		return std::move(dev.allocateCommandBuffersUnique(allocInfo).front());
	}
}