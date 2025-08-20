#pragma once

#include "pch.hpp"

class Command
{
private:
	vk::Queue queue;
	uint32_t queueIndex{};
	vk::UniqueCommandPool commandPool;
	vk::UniqueCommandBuffer commandBuffer;

public:
	vk::Queue GetQueue(vk::Device dev, uint32_t queueFamilyIndex);

	vk::UniqueCommandPool CreateCommandPool(vk::Device dev, uint32_t queueFamilyIndex);

	vk::UniqueCommandBuffer CreateCommandBuffer(vk::Device dev, vk::CommandPool commandPool);
};