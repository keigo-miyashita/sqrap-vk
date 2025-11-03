#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Device;

	class Image
	{
	private:
		const Device* pDevice_ = nullptr;
		
		static int imageIdCounter_;

		std::string name_ = "Image";

		vk::ImageCreateInfo imageCreateInfo_;
		int imageId_ = -1;

		vk::ImageAspectFlags aspectFlags_;
		vk::ImageLayout imageLayout_;
		VmaAllocation allocation_;
		VmaAllocationInfo allocationInfo_;
		vk::Image image_;

		vk::ImageViewCreateInfo imageViewCreateInfo_;
		vk::ImageView imageView_;
		std::vector<vk::ImageViewCreateInfo> mipImageViewCreateInfos_;
		std::vector<vk::ImageView> mipImageView_;
		vk::SamplerCreateInfo samplerCreateInfo_;
		vk::Sampler sampler_;

	public:
		Image(
			const Device& device,
			std::string name = "Image",
			vk::Extent3D extent3D = vk::Extent3D{ 512, 512, 1 },
			vk::ImageType imageType = vk::ImageType::e2D,
			vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eSampled,
			vk::Format format = vk::Format::eR8G8B8A8Srgb,
			vk::ImageLayout imageLayout = vk::ImageLayout::eUndefined,
			vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor,
			int mipLevels = 1,
			int arrayLayers = 1,
			vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1,
			vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
			vk::SamplerCreateInfo samplerCreateInfo = {}
		);
		Image(
			const Device& device,
			std::string name = "Image",
			vk::ImageCreateInfo imageCreateInfo = {},
			vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor,
			vk::SamplerCreateInfo samplerCreateInfo = {}
		);
		~Image();

		void Destroy();
		void Recreate(uint32_t width, uint32_t height);

		vk::Image GetImage() const;
		vk::ImageView GetImageView() const;
		vk::ImageLayout GetImageLayout() const;
		vk::ImageAspectFlags GetAspectFlags() const;
		vk::Sampler GetSampler() const;
		vk::Extent3D GetExtent3D() const;
		uint32_t GetMipLevels() const;
		uint32_t GetArrayLayers() const;
		int GetImageId() const;
		vk::Format GetFormat() const;
		vk::ImageUsageFlags GetUsage() const;
		vk::ImageView GetMipImageView(uint32_t mipLevel) const;
		std::string GetName() const;

		void SetImageLayout(vk::ImageLayout imageLayout);

	};
}