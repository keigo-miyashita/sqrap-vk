#include "Buffer.hpp"

#include "Device.hpp"

using namespace std;

namespace sqrp
{
	Buffer::Buffer(const Device& device, int size, vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags allocationFlags)
		: pDevice_(&device), size_(vk::DeviceSize(size)), usage_(usage), memoryUsage_(memoryUsage), allocationFlags_(allocationFlags)
	{
		vk::BufferCreateInfo bufferCreateInfo{};

		bufferCreateInfo
			.setSize(size_)
			.setUsage(usage_)
			.setSharingMode(vk::SharingMode::eExclusive);

		VmaAllocationCreateInfo allocCreateInfo{};
		allocCreateInfo.usage = memoryUsage_;
		allocCreateInfo.flags = allocationFlags_;

		VkBuffer buffer;
		if (vmaCreateBuffer(pDevice_->GetAllocator(), reinterpret_cast<const VkBufferCreateInfo*>(&bufferCreateInfo), &allocCreateInfo, &buffer, &allocation_, &allocationInfo_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create buffer!");
		}
		buffer_ = vk::Buffer(buffer);
		pDevice_->SetObjectName(reinterpret_cast<uint64_t>(static_cast<VkBuffer>(buffer_)), vk::ObjectType::eBuffer, "Buffer");
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

	vk::Buffer Buffer::GetBuffer() const
	{
		return buffer_;
	}

	vk::DeviceSize Buffer::GetSize() const
	{
		return size_;
	}

}