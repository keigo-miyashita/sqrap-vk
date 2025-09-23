#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Device;
	class Image;
	class Swapchain;

	class FrameBuffer
	{
	private:
		const Device* pDevice_ = nullptr;
		SwapchainHandle pSwapchain_ = nullptr;

		std::vector<vk::UniqueImageView> swapchainImageViews_;
		std::unordered_map<int, std::vector<ImageHandle>> attachmentBuffers_; // key is type of buffer

	public:
		FrameBuffer(const Device& device, SwapchainHandle pSwapchain, int numAttachmentBuffers = 0);
		~FrameBuffer() = default;
	};
}