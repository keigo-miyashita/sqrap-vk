#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Buffer;
	class Device;
	class Image;

	struct DescriptorSetCreateInfo
	{
		std::variant<BufferHandle, ImageHandle> pResource;
		vk::DescriptorType type;
		vk::ShaderStageFlags shaderStageFlags;
		//int binding = -1; // Optional: If -1, it will be set automatically based on the order in the vector
		int mipLevel = -1;
	};

	class DescriptorSet
	{
	private:
		const Device* pDevice_ = nullptr;

		std::vector<DescriptorSetCreateInfo> descriptorSetCreateInfos_;
		vk::UniqueDescriptorPool descriptorPool_;
		vk::UniqueDescriptorSetLayout descriptorSetLayout_;
		vk::UniqueDescriptorSet descriptorSets_;

	public:
		DescriptorSet(const Device& device, std::string name, std::vector<DescriptorSetCreateInfo> descriptorSetCreateInfos);
		~DescriptorSet() = default;

		vk::DescriptorSet GetDescriptorSet() const;
		vk::DescriptorSetLayout GetDescriptorSetLayout() const;
	};
}