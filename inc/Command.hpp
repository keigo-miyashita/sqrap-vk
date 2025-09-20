#pragma once

#include "pch.hpp"

#include "Device.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Device;

	class Command
	{
	private:
		const Device* pDevice_ = nullptr;
		uint32_t queueIndex = {};
		vk::UniqueCommandBuffer commandBuffer;

	public:
		Command(const Device& device, uint32_t queueFamilyIndex, uint32_t queueIndex = 0);

		vk::UniqueCommandBuffer CreateCommandBuffer(vk::Device dev, vk::CommandPool commandPool);
	};
}