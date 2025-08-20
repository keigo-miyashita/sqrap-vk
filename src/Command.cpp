#include "Command.hpp"

vk::Queue Command::GetQueue(vk::Device dev, uint32_t queueFamilyIndex)
{
	return dev.getQueue(queueFamilyIndex, queueIndex);
}

vk::UniqueCommandPool CreateCommandPool(vk::Device dev, uint32_t queueFamilyIndex)
{
	vk::CommandPoolCreateInfo createInfo{};
	createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	createInfo.setQueueFamilyIndex(queueFamilyIndex);

	return dev.createCommandPoolUnique(createInfo);
}

vk::UniqueCommandBuffer CreateCommandBuffer(vk::Device dev, vk::CommandPool commandPool)
{
	vk::CommandBufferAllocateInfo allocInfo{};
	allocInfo.setCommandPool(commandPool);
	allocInfo.setCommandBufferCount(1);
	allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);

	return std::move(dev.allocateCommandBuffersUnique(allocInfo).front());
}