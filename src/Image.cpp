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
		: pDevice_(&device), extent3D_(extent3D), imageType_(imageType), format_(format), imageLayout_(imageLayout), aspectFlags_(aspectFlags), usage_(usage), mipLevels_(mipLevels), arrayLayers_(arrayLayers)
	{
		imageId_ = imageIdCounter_;
		imageIdCounter_++;

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
			.setInitialLayout(imageLayout_);

		VmaAllocationCreateInfo allocCreateInfo{};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

		VkImage image;
		if (vmaCreateImage(pDevice_->GetAllocator(), reinterpret_cast<VkImageCreateInfo*>(&imageCreateInfo), &allocCreateInfo, &image, &allocation_, &allocationInfo_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image!");
		}
		image_ = vk::Image(image);

		pDevice_->SetObjectName((uint64_t)(VkImage)image_, vk::ObjectType::eImage, name + "Image");

		vk::ImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.setImage(image_);
		imageViewCreateInfo.setFormat(format_);
		if (imageType_ == vk::ImageType::e2D) {
			imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
			if (imageCreateInfo.flags & vk::ImageCreateFlagBits::eCubeCompatible) {
				imageViewCreateInfo.setViewType(vk::ImageViewType::eCube);
			}
		}
		else if (imageType_ == vk::ImageType::e3D) {
			imageViewCreateInfo.setViewType(vk::ImageViewType::e3D);
		}
		else if (imageType_ == vk::ImageType::e1D) {
			imageViewCreateInfo.setViewType(vk::ImageViewType::e1D);
		}
		imageViewCreateInfo.setSubresourceRange(
			vk::ImageSubresourceRange()
			.setAspectMask(aspectFlags_)
			.setBaseMipLevel(0)
			.setLevelCount(mipLevels)
			.setBaseArrayLayer(0)
			.setLayerCount(arrayLayers)
		);

		imageView_ = pDevice_->GetDevice().createImageView(imageViewCreateInfo);
		pDevice_->SetObjectName((uint64_t)(VkImageView)imageView_, vk::ObjectType::eImageView, name + "ImageView");

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
		: pDevice_(&device)
	{
		imageId_ = imageIdCounter_;
		imageIdCounter_++;

		extent3D_ = imageCreateInfo.extent;
		imageType_ = imageCreateInfo.imageType;
		format_ = imageCreateInfo.format;
		imageLayout_ = imageCreateInfo.initialLayout;
		aspectFlags_ = aspectFlags;
		mipLevels_ = imageCreateInfo.mipLevels;
		arrayLayers_ = imageCreateInfo.arrayLayers;

		VmaAllocationCreateInfo allocCreateInfo{};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

		VkImage image;
		if (vmaCreateImage(pDevice_->GetAllocator(), reinterpret_cast<VkImageCreateInfo*>(&imageCreateInfo), &allocCreateInfo, &image, &allocation_, &allocationInfo_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image!");
		}
		image_ = vk::Image(image);

		pDevice_->SetObjectName((uint64_t)(VkImage)image_, vk::ObjectType::eImage, name + "Image");

		vk::ImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.setImage(image_);
		imageViewCreateInfo.setFormat(format_);
		if (imageType_ == vk::ImageType::e2D) {
			imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
			if (imageCreateInfo.flags & vk::ImageCreateFlagBits::eCubeCompatible) {
				imageViewCreateInfo.setViewType(vk::ImageViewType::eCube);
			}
		}
		else if (imageType_ == vk::ImageType::e3D) {
			imageViewCreateInfo.setViewType(vk::ImageViewType::e3D);
		}
		else if (imageType_ == vk::ImageType::e1D) {
			imageViewCreateInfo.setViewType(vk::ImageViewType::e1D);
		}
		imageViewCreateInfo.setSubresourceRange(
			vk::ImageSubresourceRange()
			.setAspectMask(aspectFlags_)
			.setBaseMipLevel(0)
			.setLevelCount(imageCreateInfo.mipLevels)
			.setBaseArrayLayer(0)
			.setLayerCount(imageCreateInfo.arrayLayers)
		);

		imageView_ = pDevice_->GetDevice().createImageView(imageViewCreateInfo);
		pDevice_->SetObjectName((uint64_t)(VkImageView)imageView_, vk::ObjectType::eImageView, name + "ImageView");

		if (mipLevels_ > 1) {
			mipImageView_.resize(mipLevels_);
			for (uint32_t i = 0; i < mipLevels_; i++) {
				vk::ImageViewCreateInfo mipViewCreateInfo{};
				mipViewCreateInfo.setImage(image_);
				mipViewCreateInfo.setFormat(format_);
				if (imageType_ == vk::ImageType::e2D) {
					mipViewCreateInfo.setViewType(vk::ImageViewType::e2D);
					if (imageCreateInfo.flags & vk::ImageCreateFlagBits::eCubeCompatible) {
						mipViewCreateInfo.setViewType(vk::ImageViewType::eCube);
					}
				}
				else if (imageType_ == vk::ImageType::e3D) {
					mipViewCreateInfo.setViewType(vk::ImageViewType::e3D);
				}
				else if (imageType_ == vk::ImageType::e1D) {
					mipViewCreateInfo.setViewType(vk::ImageViewType::e1D);
				}
				mipViewCreateInfo.setSubresourceRange(
					vk::ImageSubresourceRange()
					.setAspectMask(aspectFlags_)
					.setBaseMipLevel(i)
					.setLevelCount(1)
					.setBaseArrayLayer(0)
					.setLayerCount(imageCreateInfo.arrayLayers)
				);
				mipImageView_[i] = pDevice_->GetDevice().createImageView(mipViewCreateInfo);
				pDevice_->SetObjectName((uint64_t)(VkImageView)mipImageView_[i], vk::ObjectType::eImageView, name + "MipImageView_" + to_string(i));
			}
		}

		sampler_ = pDevice_->GetDevice().createSampler(samplerCreateInfo);
		pDevice_->SetObjectName((uint64_t)(VkSampler)sampler_, vk::ObjectType::eSampler, name + "Sampler");
	}

	Image::~Image()
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

	/*void Image::Recreate(uint32_t width, uint32_t height)
	{
		if (imageView_) {
			pDevice_->GetDevice().destroyImageView(imageView_);
		}
		if (sampler_) {
			pDevice_->GetDevice().destroySampler(sampler_);
		}

		vmaDestroyImage(pDevice_->GetAllocator(), image_, allocation_);
	}*/

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
		return extent3D_;
	}

	uint32_t Image::GetMipLevels() const
	{
		return mipLevels_;
	}

	uint32_t Image::GetArrayLayers() const
	{
		return arrayLayers_;
	}

	int Image::GetImageId() const
	{
		return imageId_;
	}

	vk::Format Image::GetFormat() const
	{
		return format_;
	}

	vk::ImageUsageFlags Image::GetUsage() const
	{
		return usage_;
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