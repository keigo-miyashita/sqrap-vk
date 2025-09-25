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
		vk::BufferUsageFlags usage_;
		VmaMemoryUsage memoryUsage_;
		VmaAllocationCreateFlags allocationFlags_;

	public:
		Buffer(const Device& device, int size, vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, VmaAllocationCreateFlags allocationFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);
		~Buffer();
		
		void* Map();
		void Unmap();
		void Copy();

		vk::Buffer GetBuffer() const;
		vk::DeviceSize GetSize() const;
	};
}