#include "Buffer.hpp"

#include "Device.hpp"

using namespace std;

namespace sqrp
{
	Buffer::Buffer(const Device& device, int size, vk::BufferUsageFlagBits usage)
		: pDevice_(&device), size_(vk::DeviceSize(size))
	{
		vk::BufferCreateInfo bufferCreateInfo{};

		bufferCreateInfo
			.setSize(size_)
			.setUsage(usage)
			.setSharingMode(vk::SharingMode::eExclusive);

		VmaAllocationCreateInfo allocCreateInfo{};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		allocCreateInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

		VkBuffer buffer;
		if (vmaCreateBuffer(pDevice_->GetAllocator(), reinterpret_cast<const VkBufferCreateInfo*>(&bufferCreateInfo), &allocCreateInfo, &buffer, &allocation_, &allocationInfo_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create buffer!");
		}
		buffer_ = vk::Buffer(buffer);
	}

	Buffer::~Buffer()
	{
		vmaDestroyBuffer(pDevice_->GetAllocator(), buffer_, allocation_);
	}

	void* Buffer::Map()
	{
		void* data = nullptr;
		if (vmaMapMemory(pDevice_->GetAllocator(), allocation_, &data) != VK_SUCCESS) {
			throw std::runtime_error("Failed to map buffer memory");
		}
		return data;
	}

	void Buffer::Unmap()
	{
		vmaUnmapMemory(pDevice_->GetAllocator(), allocation_);
	}


}