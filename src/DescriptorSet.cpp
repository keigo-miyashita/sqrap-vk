#include "DescriptorSet.hpp"

#include "Buffer.hpp"
#include "CommandBuffer.hpp"
#include "Device.hpp"
#include "Image.hpp"

using namespace std;

namespace sqrp
{
	DescriptorSet::DescriptorSet(const Device& device, std::string name, std::vector<DescriptorSetCreateInfo> descriptorSetCreateInfos)
		: pDevice_(&device), descriptorSetCreateInfos_(descriptorSetCreateInfos)
	{
		// Create Descriptor Set Layout
		vector<vk::DescriptorSetLayoutBinding> layoutBindings(descriptorSetCreateInfos_.size());
		map<vk::DescriptorType, uint32_t> descriptorTypeCounts;
		int index = 0;
		for (const auto& descriptorSetCreateInfo : descriptorSetCreateInfos_) {
			auto type = descriptorSetCreateInfo.type;
			layoutBindings[index] = vk::DescriptorSetLayoutBinding{}
				//.setBinding((descriptorSetCreateInfo.binding == -1) ? index : descriptorSetCreateInfo.binding)
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
			throw std::runtime_error("Failed to create descriptor set layout");
		} else {
			pDevice_->SetObjectName(
				(uint64_t)(VkDescriptorSetLayout)(descriptorSetLayout_.get()),
				vk::ObjectType::eDescriptorSetLayout,
				name + "_DescriptorSetLayout"
			);
		}

		// Create Descriptor Pool
		vector<vk::DescriptorPoolSize> poolSizes(descriptorTypeCounts.size());
		index = 0;
		for (const auto& [type, count] : descriptorTypeCounts) {
			poolSizes[index] = vk::DescriptorPoolSize{}
				.setType(type)
				.setDescriptorCount(count);
			index++;
		};

		// Decriptor Pool
		descriptorPool_ = pDevice_->GetDevice().createDescriptorPoolUnique(vk::DescriptorPoolCreateInfo{}
			.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
			.setMaxSets(1)
			.setPoolSizeCount(static_cast<uint32_t>(poolSizes.size()))
			.setPPoolSizes(poolSizes.data())
		);
		pDevice_->SetObjectName(
			(uint64_t)(VkDescriptorPool)(descriptorPool_.get()),
			vk::ObjectType::eDescriptorPool,
			name + "_DescriptorPool"
		);

		// Create Descriptor Sets
		descriptorSets_ = std::move(pDevice_->GetDevice().allocateDescriptorSetsUnique(vk::DescriptorSetAllocateInfo{}
			.setDescriptorPool(descriptorPool_.get())
			.setDescriptorSetCount(1)
			.setPSetLayouts(&descriptorSetLayout_.get())
		).front());
		pDevice_->SetObjectName(
			(uint64_t)(VkDescriptorSet)(descriptorSets_.get()),
			vk::ObjectType::eDescriptorSet,
			name + "_DescriptorSet"
		);

		std::vector<vk::WriteDescriptorSet> writeDescriptorSets(descriptorSetCreateInfos_.size());
		std::vector<vk::DescriptorBufferInfo> descriptorBufferInfos(descriptorSetCreateInfos_.size());
		std::vector<vk::DescriptorImageInfo> descriptorImageInfos(descriptorSetCreateInfos_.size());
		index = 0;
		for (const auto& descriptorSetCreateInfo : descriptorSetCreateInfos_) {
			if (std::holds_alternative<BufferHandle>(descriptorSetCreateInfo.pResource)) {
				auto buffer = std::get<BufferHandle>(descriptorSetCreateInfo.pResource);
				descriptorBufferInfos[index].setBuffer(buffer->GetBuffer());
				descriptorBufferInfos[index].setOffset(0);
				descriptorBufferInfos[index].setRange(buffer->GetSize());

				writeDescriptorSets[index] = vk::WriteDescriptorSet{}
					.setDstSet(descriptorSets_.get())
					//.setDstBinding((descriptorSetCreateInfo.binding == -1) ? index : descriptorSetCreateInfo.binding)
					.setDstBinding(index)
					.setDescriptorType(descriptorSetCreateInfo.type)
					.setDescriptorCount(1)
					.setPBufferInfo(&descriptorBufferInfos[index]);
			}
			else if (std::holds_alternative<ImageHandle>(descriptorSetCreateInfo.pResource)) {
				auto image = std::get<ImageHandle>(descriptorSetCreateInfo.pResource);
				cout << "image name: " << image->GetName() << endl;
				if (descriptorSetCreateInfo.type == vk::DescriptorType::eCombinedImageSampler) {
					descriptorImageInfos[index].setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal); // Readonly for shader
				}
				else if (descriptorSetCreateInfo.type == vk::DescriptorType::eStorageImage) {
					descriptorImageInfos[index].setImageLayout(vk::ImageLayout::eGeneral); // Readwrite for shader
				}
				if (descriptorSetCreateInfo.mipLevel == -1) {
					descriptorImageInfos[index].setImageView(image->GetImageView());
				}
				else {
					descriptorImageInfos[index].setImageView(image->GetMipImageView(descriptorSetCreateInfo.mipLevel));
				}
				descriptorImageInfos[index].setSampler(image->GetSampler());

				writeDescriptorSets[index] = vk::WriteDescriptorSet{}
					.setDstSet(descriptorSets_.get())
					//.setDstBinding((descriptorSetCreateInfo.binding == -1) ? index : descriptorSetCreateInfo.binding)
					.setDstBinding(index)
					.setDescriptorType(descriptorSetCreateInfo.type)
					.setDescriptorCount(1)
					.setPImageInfo(&descriptorImageInfos[index]);
			}
			else {
				throw std::runtime_error("Invalid resource type in DescriptorSetCreateInfo");
			}
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