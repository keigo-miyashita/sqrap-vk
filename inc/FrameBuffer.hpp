#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Device;
	class Image;
	class RenderPass;
	class Swapchain;

	class FrameBuffer
	{
	private:
		const Device* pDevice_ = nullptr;
		SwapchainHandle pSwapchain_ = nullptr;
		RenderPassHandle pRenderPass_ = nullptr;

		std::vector<vk::UniqueImageView> swapchainImageViews_;
		std::unordered_map<int, std::vector<ImageHandle>> attachmentBuffers_; // key is type of buffer
		std::vector<vk::UniqueFramebuffer> framebuffers_;

	public:
		FrameBuffer(const Device& device, SwapchainHandle pSwapchain, RenderPassHandle pRenderPass, int numAttachmentBuffers = 0);
		~FrameBuffer() = default;

		vk::Framebuffer GetFrameBuffer(int index) const;
		SwapchainHandle GetSwapchain() const;
	};
}