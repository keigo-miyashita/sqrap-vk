#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Device;

	class Buffer
	{
	private:
		const Device* pDevice_ = nullptr;

		vk::Buffer buffer_;
		VmaAllocation allocation_;
		VmaAllocationInfo allocationInfo_;
		vk::DeviceSize size_ = 0;

	public:
		Buffer(const Device& device, int size, vk::BufferUsageFlagBits usage);
		~Buffer();
		
		void* Map();
		void Unmap();
	};
}