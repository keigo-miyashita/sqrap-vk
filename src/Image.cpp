#include "Image.hpp"

#include "Device.hpp"

using namespace std;

namespace sqrp
{
	int Image::imageIdCounter_ = 0;

	Image::Image(
		const Device& device,
		std::string name,
		vk::Extent3D extent3D,
		vk::ImageType imageType,
		vk::ImageUsageFlags usage,
		vk::Format format,
		vk::ImageLayout imageLayout,
		vk::ImageAspectFlags aspectFlags,
		int mipLevels,
		int arrayLayers,
		vk::SampleCountFlagBits samples,
		vk::ImageTiling tiling,
		vk::SamplerCreateInfo samplerCreateInfo
	)
		: pDevice_(&device), name_(name), aspectFlags_(aspectFlags)/*, extent3D_(extent3D), imageType_(imageType), format_(format), imageLayout_(imageLayout), aspectFlags_(aspectFlags), usage_(usage), mipLevels_(mipLevels), arrayLayers_(arrayLayers)*/
	{
		imageId_ = imageIdCounter_;
		imageIdCounter_++;

		imageCreateInfo_
			.setExtent(extent3D)
			.setImageType(imageType)
			.setUsage(usage)
			.setFormat(format)
			.setMipLevels(mipLevels)
			.setArrayLayers(arrayLayers)
			.setSamples(samples)
			.setTiling(tiling)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setInitialLayout(imageLayout);

		VmaAllocationCreateInfo allocCreateInfo{};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

		//imageCreateInfo_ = imageCreateInfo;
		VkImage image;
		if (vmaCreateImage(pDevice_->GetAllocator(), reinterpret_cast<VkImageCreateInfo*>(&imageCreateInfo_), &allocCreateInfo, &image, &allocation_, &allocationInfo_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image!");
		}
		image_ = vk::Image(image);

		pDevice_->SetObjectName((uint64_t)(VkImage)image_, vk::ObjectType::eImage, name + "Image");

		imageViewCreateInfo_.setImage(image_);
		imageViewCreateInfo_.setFormat(format);
		if (imageType == vk::ImageType::e2D) {
			imageViewCreateInfo_.setViewType(vk::ImageViewType::e2D);
			if (imageCreateInfo_.flags & vk::ImageCreateFlagBits::eCubeCompatible) {
				imageViewCreateInfo_.setViewType(vk::ImageViewType::eCube);
			}
		}
		else if (imageType == vk::ImageType::e3D) {
			imageViewCreateInfo_.setViewType(vk::ImageViewType::e3D);
		}
		else if (imageType == vk::ImageType::e1D) {
			imageViewCreateInfo_.setViewType(vk::ImageViewType::e1D);
		}
		imageViewCreateInfo_.setSubresourceRange(
			vk::ImageSubresourceRange()
			.setAspectMask(aspectFlags)
			.setBaseMipLevel(0)
			.setLevelCount(mipLevels)
			.setBaseArrayLayer(0)
			.setLayerCount(arrayLayers)
		);

		//imageViewCreateInfo_ = imageViewCreateInfo;
		imageView_ = pDevice_->GetDevice().createImageView(imageViewCreateInfo_);
		pDevice_->SetObjectName((uint64_t)(VkImageView)imageView_, vk::ObjectType::eImageView, name + "ImageView");

		samplerCreateInfo_ = samplerCreateInfo;
		sampler_ = pDevice_->GetDevice().createSampler(samplerCreateInfo);
		pDevice_->SetObjectName((uint64_t)(VkSampler)sampler_, vk::ObjectType::eSampler, name + "Sampler");
	}

	Image::Image(
		const Device& device,
		std::string name,
		vk::ImageCreateInfo imageCreateInfo,
		vk::ImageAspectFlags aspectFlags,
		vk::SamplerCreateInfo samplerCreateInfo
	)
		: pDevice_(&device), name_(name), aspectFlags_(aspectFlags)
	{
		imageId_ = imageIdCounter_;
		imageIdCounter_++;

		imageLayout_ = imageCreateInfo.initialLayout;

		VmaAllocationCreateInfo allocCreateInfo{};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

		imageCreateInfo_ = imageCreateInfo;
		VkImage image;
		if (vmaCreateImage(pDevice_->GetAllocator(), reinterpret_cast<VkImageCreateInfo*>(&imageCreateInfo_), &allocCreateInfo, &image, &allocation_, &allocationInfo_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image!");
		}
		image_ = vk::Image(image);

		pDevice_->SetObjectName((uint64_t)(VkImage)image_, vk::ObjectType::eImage, name + "Image");

		imageViewCreateInfo_.setImage(image_);
		imageViewCreateInfo_.setFormat(imageCreateInfo_.format);
		if (imageCreateInfo_.imageType == vk::ImageType::e2D) {
			imageViewCreateInfo_.setViewType(vk::ImageViewType::e2D);
			if (imageCreateInfo_.flags & vk::ImageCreateFlagBits::eCubeCompatible) {
				imageViewCreateInfo_.setViewType(vk::ImageViewType::eCube);
			}
		}
		else if (imageCreateInfo_.imageType == vk::ImageType::e3D) {
			imageViewCreateInfo_.setViewType(vk::ImageViewType::e3D);
		}
		else if (imageCreateInfo_.imageType == vk::ImageType::e1D) {
			imageViewCreateInfo_.setViewType(vk::ImageViewType::e1D);
		}
		imageViewCreateInfo_.setSubresourceRange(
			vk::ImageSubresourceRange()
			.setAspectMask(aspectFlags)
			.setBaseMipLevel(0)
			.setLevelCount(imageCreateInfo_.mipLevels)
			.setBaseArrayLayer(0)
			.setLayerCount(imageCreateInfo_.arrayLayers)
		);

		imageView_ = pDevice_->GetDevice().createImageView(imageViewCreateInfo_);
		pDevice_->SetObjectName((uint64_t)(VkImageView)imageView_, vk::ObjectType::eImageView, name + "ImageView");

		if (imageCreateInfo_.mipLevels > 1) {
			mipImageViewCreateInfos_.resize(imageCreateInfo_.mipLevels);
			mipImageView_.resize(imageCreateInfo_.mipLevels);
			for (uint32_t i = 0; i < imageCreateInfo_.mipLevels; i++) {
				vk::ImageViewCreateInfo mipViewCreateInfo{};
				mipViewCreateInfo.setImage(image_);
				mipViewCreateInfo.setFormat(imageCreateInfo_.format);
				if (imageCreateInfo_.imageType == vk::ImageType::e2D) {
					mipViewCreateInfo.setViewType(vk::ImageViewType::e2D);
					if (imageCreateInfo.flags & vk::ImageCreateFlagBits::eCubeCompatible) {
						mipViewCreateInfo.setViewType(vk::ImageViewType::eCube);
					}
				}
				else if (imageCreateInfo_.imageType == vk::ImageType::e3D) {
					mipViewCreateInfo.setViewType(vk::ImageViewType::e3D);
				}
				else if (imageCreateInfo_.imageType == vk::ImageType::e1D) {
					mipViewCreateInfo.setViewType(vk::ImageViewType::e1D);
				}
				mipViewCreateInfo.setSubresourceRange(
					vk::ImageSubresourceRange()
					.setAspectMask(aspectFlags)
					.setBaseMipLevel(i)
					.setLevelCount(1)
					.setBaseArrayLayer(0)
					.setLayerCount(imageCreateInfo.arrayLayers)
				);
				mipImageViewCreateInfos_[i] = mipViewCreateInfo;
				mipImageView_[i] = pDevice_->GetDevice().createImageView(mipViewCreateInfo);
				pDevice_->SetObjectName((uint64_t)(VkImageView)mipImageView_[i], vk::ObjectType::eImageView, name + "MipImageView_" + to_string(i));
			}
		}

		samplerCreateInfo_ = samplerCreateInfo;
		sampler_ = pDevice_->GetDevice().createSampler(samplerCreateInfo);
		pDevice_->SetObjectName((uint64_t)(VkSampler)sampler_, vk::ObjectType::eSampler, name + "Sampler");
	}

	Image::~Image()
	{
		Destroy();
	}

	void Image::Destroy()
	{
		if (imageView_) {
			pDevice_->GetDevice().destroyImageView(imageView_);
		}
		if (sampler_) {
			pDevice_->GetDevice().destroySampler(sampler_);
		}
		if (mipImageView_.size() > 0) {
			for (auto& mipView : mipImageView_) {
				pDevice_->GetDevice().destroyImageView(mipView);
			}
		}

		vmaDestroyImage(pDevice_->GetAllocator(), image_, allocation_);
	}

	void Image::Recreate(uint32_t width, uint32_t height)
	{
		Destroy();

		imageCreateInfo_.setExtent(
			vk::Extent3D{
				width,
				height,
				imageCreateInfo_.extent.depth
			}
		);

		VmaAllocationCreateInfo allocCreateInfo{};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

		VkImage image;
		if (vmaCreateImage(pDevice_->GetAllocator(), reinterpret_cast<VkImageCreateInfo*>(&imageCreateInfo_), &allocCreateInfo, &image, &allocation_, &allocationInfo_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image!");
		}
		image_ = vk::Image(image);

		pDevice_->SetObjectName((uint64_t)(VkImage)image_, vk::ObjectType::eImage, name_ + "Image");

		//vk::ImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo_.setImage(image_);

		imageView_ = pDevice_->GetDevice().createImageView(imageViewCreateInfo_);
		pDevice_->SetObjectName((uint64_t)(VkImageView)imageView_, vk::ObjectType::eImageView, name_ + "ImageView");

		if (imageCreateInfo_.mipLevels > 1) {
			mipImageView_.resize(imageCreateInfo_.mipLevels);
			for (uint32_t i = 0; i < imageCreateInfo_.mipLevels; i++) {
				vk::ImageViewCreateInfo mipViewCreateInfo{};
				mipViewCreateInfo.setImage(image_);
				mipImageView_[i] = pDevice_->GetDevice().createImageView(mipViewCreateInfo);
				pDevice_->SetObjectName((uint64_t)(VkImageView)mipImageView_[i], vk::ObjectType::eImageView, name_ + "MipImageView_" + to_string(i));
			}
		}

		sampler_ = pDevice_->GetDevice().createSampler(samplerCreateInfo_);
		pDevice_->SetObjectName((uint64_t)(VkSampler)sampler_, vk::ObjectType::eSampler, name_ + "Sampler");
	}

	vk::Image Image::GetImage() const
	{
		return image_;
	}

	vk::ImageView Image::GetImageView() const
	{
		return imageView_;
	}

	vk::ImageLayout Image::GetImageLayout() const
	{
		return imageLayout_;
	}

	vk::ImageAspectFlags Image::GetAspectFlags() const
	{
		return aspectFlags_;
	}

	vk::Sampler Image::GetSampler() const
	{
		return sampler_;
	}

	vk::Extent3D Image::GetExtent3D() const
	{
		return imageCreateInfo_.extent;
	}

	uint32_t Image::GetMipLevels() const
	{
		return imageCreateInfo_.mipLevels;
	}

	uint32_t Image::GetArrayLayers() const
	{
		return imageCreateInfo_.arrayLayers;
	}

	int Image::GetImageId() const
	{
		return imageId_;
	}

	vk::Format Image::GetFormat() const
	{
		return imageCreateInfo_.format;
	}

	vk::ImageUsageFlags Image::GetUsage() const
	{
		return imageCreateInfo_.usage;
	}

	vk::ImageView Image::GetMipImageView(uint32_t mipLevel) const
	{
		return mipImageView_[mipLevel];
	}

	void Image::SetImageLayout(vk::ImageLayout imageLayout)
	{
		imageLayout_ = imageLayout;
	}
}