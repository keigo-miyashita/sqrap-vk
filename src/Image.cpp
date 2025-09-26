#include "Image.hpp"

#include "Device.hpp"

using namespace std;

namespace sqrp
{
	Image::Image(
		const Device& device,
		vk::Extent3D extent3D,
		vk::ImageType imageType,
		vk::ImageUsageFlags usage,
		vk::Format format,
		int mipLevels,
		int arrayLayers,
		vk::SampleCountFlagBits samples,
		vk::ImageTiling tiling,
		vk::SamplerCreateInfo samplerCreateInfo
	)
		: pDevice_(&device), extent3D_(extent3D), imageType_(imageType), format_(format)
	{
		vk::ImageCreateInfo imageCreateInfo{};
		
		imageCreateInfo
			.setExtent(extent3D_)
			.setImageType(imageType_)
			.setUsage(usage)
			.setFormat(format_)
			.setMipLevels(mipLevels)
			.setArrayLayers(arrayLayers)
			.setSamples(samples)
			.setTiling(tiling)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setInitialLayout(vk::ImageLayout::eUndefined);

		VmaAllocationCreateInfo allocCreateInfo{};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

		VkImage image;
		if (vmaCreateImage(pDevice_->GetAllocator(), reinterpret_cast<VkImageCreateInfo*>(&imageCreateInfo), &allocCreateInfo, &image, &allocation_, &allocationInfo_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image!");
		}
		image_ = vk::Image(image);

		vk::ImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.setImage(image_);
		imageViewCreateInfo.setFormat(format_);
		if (imageType_ == vk::ImageType::e2D) {
			imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
		}
		else if (imageType_ == vk::ImageType::e3D) {
			imageViewCreateInfo.setViewType(vk::ImageViewType::e3D);
		}
		else if (imageType_ == vk::ImageType::e1D) {
			imageViewCreateInfo.setViewType(vk::ImageViewType::e1D);
		}
		imageViewCreateInfo.setSubresourceRange(
			vk::ImageSubresourceRange()
			.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseMipLevel(0)
			.setLevelCount(mipLevels)
			.setBaseArrayLayer(0)
			.setLayerCount(arrayLayers)
		);

		imageView_ = pDevice_->GetDevice().createImageView(imageViewCreateInfo);

		sampler_ = pDevice_->GetDevice().createSampler(samplerCreateInfo);
	}

	Image::~Image()
	{
		if (imageView_) {
			pDevice_->GetDevice().destroyImageView(imageView_);
		}
		if (sampler_) {
			pDevice_->GetDevice().destroySampler(sampler_);
		}

		vmaDestroyImage(pDevice_->GetAllocator(), image_, allocation_);
	}


}