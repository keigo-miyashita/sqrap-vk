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
		RenderPassHandle pRenderPass_ = nullptr;
		std::optional<SwapchainHandle> pSwapchain_ = std::nullopt;

		std::string name_ = "FrameBuffer";
		
		int inflightCount_ = 3;

		std::vector<std::vector<ImageHandle>> attachmentImages_;
		std::optional<std::vector<vk::UniqueImageView>> swapchainImageViews_;
		std::vector<vk::UniqueFramebuffer> framebuffers_;

		uint32_t width_ = 0;
		uint32_t height_ = 0;

	public:
		FrameBuffer(const Device& device, std::string name, RenderPassHandle pRenderPass, SwapchainHandle pSwapchain, std::vector<ImageHandle> depthImages = {});
		FrameBuffer(const Device& device, std::string name, RenderPassHandle pRenderPass, std::vector<std::vector<ImageHandle>> attachmentImages, uint32_t width, uint32_t height, int inflightCount, SwapchainHandle pSwapchain = nullptr);
		~FrameBuffer() = default;

		void Recreate(uint32_t width, uint32_t height);
		vk::Framebuffer GetFrameBuffer(int index) const;
		SwapchainHandle GetSwapchain() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
	};
}