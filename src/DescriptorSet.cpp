#include "DescriptorSet.hpp"

#include "Buffer.hpp"
#include "Device.hpp"

using namespace std;

namespace sqrp
{
	DescriptorSet::DescriptorSet(const Device& device, std::vector<DescriptorSetCreateInfo> descriptorSetCreateInfos)
		: pDevice_(&device), descriptorSetCreateInfos_(descriptorSetCreateInfos)
	{
		vector<vk::DescriptorSetLayoutBinding> layoutBindings(descriptorSetCreateInfos_.size());
		map<vk::DescriptorType, uint32_t> descriptorTypeCounts;
		int index = 0;
		for (const auto& descriptorSetCreateInfo : descriptorSetCreateInfos_) {
			auto type = descriptorSetCreateInfo.type;
			layoutBindings[index] = vk::DescriptorSetLayoutBinding{}
				.setBinding(index)
				.setDescriptorType(type)
				.setDescriptorCount(1)
				.setStageFlags(descriptorSetCreateInfo.shaderStageFlags);

			if (descriptorTypeCounts.find(type) != descriptorTypeCounts.end()) {
				descriptorTypeCounts[type]++;
			}
			else {
				descriptorTypeCounts[type] = 1;
			}
			index++;
		}
		
		descriptorSetLayout_ = pDevice_->GetDevice().createDescriptorSetLayoutUnique(vk::DescriptorSetLayoutCreateInfo{}
			.setBindingCount(static_cast<uint32_t>(layoutBindings.size()))
			.setPBindings(layoutBindings.data())
		);
		if (!descriptorSetLayout_) {
			cout << "Failed to create descriptor set layout" << endl;
		}
		else {
			cout << "Successed to create descriptor set layout" << endl;
		}
		vector<vk::DescriptorPoolSize> poolSizes(descriptorTypeCounts.size());
		index = 0;
		for (const auto& [type, count] : descriptorTypeCounts) {
			poolSizes[index] = vk::DescriptorPoolSize{}
				.setType(type)
				.setDescriptorCount(count);
		};

		cout << "poolSizes.size() = " << poolSizes.size() << endl;
		descriptorPool_ = pDevice_->GetDevice().createDescriptorPoolUnique(vk::DescriptorPoolCreateInfo{}
			.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
			.setMaxSets(1)
			.setPoolSizeCount(static_cast<uint32_t>(poolSizes.size()))
			.setPPoolSizes(poolSizes.data())
		);

		descriptorSets_ = std::move(pDevice_->GetDevice().allocateDescriptorSetsUnique(vk::DescriptorSetAllocateInfo{}
			.setDescriptorPool(descriptorPool_.get())
			.setDescriptorSetCount(1)
			.setPSetLayouts(&descriptorSetLayout_.get())
		).front());

		std::vector<vk::WriteDescriptorSet> writeDescriptorSets(descriptorSetCreateInfos_.size());
		index = 0;
		for (const auto& descriptorSetCreateInfo : descriptorSetCreateInfos_) {
			vk::DescriptorBufferInfo bufferInfo = {};
			bufferInfo.setBuffer(descriptorSetCreateInfo.pBuffer->GetBuffer());
			bufferInfo.setOffset(0);
			bufferInfo.setRange(descriptorSetCreateInfo.pBuffer->GetSize());
			writeDescriptorSets[index] = vk::WriteDescriptorSet{}
				.setDstSet(descriptorSets_.get())
				.setDstBinding(index)
				.setDescriptorType(descriptorSetCreateInfo.type)
				.setDescriptorCount(1)
				.setPBufferInfo(&bufferInfo);
			index++;
		};
		pDevice_->GetDevice().updateDescriptorSets(static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);

	}

	vk::DescriptorSet DescriptorSet::GetDescriptorSet() const
	{
		return descriptorSets_.get();
	}

	vk::DescriptorSetLayout DescriptorSet::GetDescriptorSetLayout() const
	{
		return descriptorSetLayout_.get();
	}
}