#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Device;
	class Image;
	class RenderPass;
	class Swapchain;

	struct FrameBufferInfo
	{
		vk::Format format;
		vk::ImageUsageFlags imageUsage;
		std::string debugName;
	};

	class FrameBuffer
	{
	private:
		const Device* pDevice_ = nullptr;
		RenderPassHandle pRenderPass_ = nullptr;
		std::optional<SwapchainHandle> pSwapchain_ = std::nullopt;
		
		int inflightCount_ = 3;
		std::vector<FrameBufferInfo> frameBufferInfos_;

		std::optional<std::vector<vk::UniqueImageView>> swapchainImageViews_;
		std::unordered_map<int, std::vector<ImageHandle>> attachmentImages_; // key is type of buffer
		std::vector<vk::UniqueFramebuffer> framebuffers_;

		uint32_t width_ = 0;
		uint32_t height_ = 0;

	public:
		FrameBuffer(const Device& device, RenderPassHandle pRenderPass, SwapchainHandle pSwapchain, bool useDepth = true);
		FrameBuffer(const Device& device, RenderPassHandle pRenderPass, std::vector<FrameBufferInfo> frameBufferInfos, uint32_t width, uint32_t height, int inflightCount, SwapchainHandle pSwapchain = nullptr);
		~FrameBuffer() = default;

		void Recreate(uint32_t width, uint32_t height);
		vk::Framebuffer GetFrameBuffer(int index) const;
		SwapchainHandle GetSwapchain() const;
		ImageHandle GetAttachmentImage(int index, int inflightIndex) const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
	};
}