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
		vk::DeviceSize size_ = 0;
		vk::BufferUsageFlags usage_;
		VmaAllocation allocation_;
		VmaAllocationInfo allocationInfo_;
		VmaMemoryUsage memoryUsage_;
		VmaAllocationCreateFlags allocationFlags_;

	public:
		Buffer(const Device& device, std::string name, int size, vk::BufferUsageFlags usage, VmaAllocationCreateFlags allocationFlags, VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
		~Buffer();
		
		void* Map();
		void Unmap();
		void Copy();
		template<typename T>
		void Write(const T& data)
		{
			void* rawPtr = Map();
			*static_cast<T*>(rawPtr) = data;

			Unmap();
		}
		void Write(const void* src, size_t size);

		vk::Buffer GetBuffer() const;
		vk::DeviceSize GetSize() const;
	};
}