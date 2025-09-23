#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class CommandBuffer;
	class Device;

	class Swapchain
	{
	private:
		const Device* pDevice_ = nullptr;

		uint32_t minImageCount_ = 3;
		uint32_t imageIndex_ = 0;

		uint32_t inflightCount_ = 3;
		uint32_t inflightIndex_ = 0;

		vk::SurfaceCapabilitiesKHR capabilities_;
		vk::SurfaceFormatKHR surfaceFormat_;
		vk::PresentModeKHR presentMode_;
		vk::UniqueSwapchainKHR swapchain_;
		uint32_t width_;
		uint32_t height_;
		// Swapchain images are managed by the swapchain itself
		// so are not managed by user and you should not use vk::UniqueImage
		std::vector<vk::Image> swapchainImages_;
		//std::vector<vk::UniqueImageView> swapchainImageViews_;
		std::vector<CommandBufferHandle> graphicsCommandBuffers_;
		std::vector<CommandBufferHandle> computeCommandBuffers_;

	public:
		Swapchain(const Device& device, uint32_t width, uint32_t height);
		~Swapchain() = default;

		const std::vector<vk::Image>& GetSwapchainImages() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		vk::Format GetSurfaceFormat() const;
		uint32_t GetInflightCount() const;
	};
}