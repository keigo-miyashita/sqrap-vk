#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Buffer;
	class Device;

	struct DescriptorSetCreateInfo
	{
		BufferHandle pBuffer = nullptr;
		vk::DescriptorType type;
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
		DescriptorSet(const Device& device, std::vector<DescriptorSetCreateInfo> descriptorSetCreateInfos, vk::ShaderStageFlags shaderStageFlags);
		~DescriptorSet() = default;

		vk::DescriptorSetLayout GetDescriptorSetLayout() const;
	};
}