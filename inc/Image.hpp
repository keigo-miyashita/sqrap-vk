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

		vk::Image image_;
		vk::Extent3D extent3D_;
		vk::ImageType imageType_;
		vk::Format format_;
		VmaAllocation allocation_;
		VmaAllocationInfo allocationInfo_;

		vk::ImageView imageView_;
		vk::Sampler sampler_;

	public:
		Image(
			const Device& device,
			vk::Extent3D extent3D = vk::Extent3D{ 512, 512, 1 },
			vk::ImageType imageType = vk::ImageType::e2D,
			vk::ImageUsageFlagBits usage = vk::ImageUsageFlagBits::eSampled,
			vk::Format format = vk::Format::eR8G8B8A8Srgb,
			int mipLevels = 1,
			int arrayLayers = 1,
			vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1,
			vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
			vk::SamplerCreateInfo samplerCreateInfo = {}
		);
		~Image();


	};
}