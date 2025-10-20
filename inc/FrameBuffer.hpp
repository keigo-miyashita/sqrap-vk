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
		
		int inflightCount_ = 3;
		//std::vector<FrameBufferInfo> frameBufferInfos_;

		std::optional<std::vector<vk::UniqueImageView>> swapchainImageViews_;
		//std::unordered_map<int, std::vector<ImageHandle>> attachmentImages_; // key is type of buffer
		std::vector<std::vector<ImageHandle>> attachmentImages_; // index is index of frameBufferInfos_
		std::vector<vk::UniqueFramebuffer> framebuffers_;

		uint32_t width_ = 0;
		uint32_t height_ = 0;

	public:
		FrameBuffer(const Device& device, RenderPassHandle pRenderPass, SwapchainHandle pSwapchain, std::vector<ImageHandle> depthImages = {});
		FrameBuffer(const Device& device, RenderPassHandle pRenderPass, std::vector<std::vector<ImageHandle>> attachmentImages, uint32_t width, uint32_t height, int inflightCount, SwapchainHandle pSwapchain = nullptr);
		~FrameBuffer() = default;

		void Recreate(uint32_t width, uint32_t height);
		vk::Framebuffer GetFrameBuffer(int index) const;
		SwapchainHandle GetSwapchain() const;
		//ImageHandle GetAttachmentImage(int index, int inflightIndex) const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
	};
}