#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class CommandBuffer;
	class Device;
	class Fence;
	class Semaphore;

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
		uint32_t width_;
		uint32_t height_;

		vk::UniqueSwapchainKHR swapchain_;
		// Swapchain images are managed by the swapchain itself
		// so are not managed by user and you should not use vk::UniqueImage
		std::vector<vk::Image> swapchainImages_;
		std::vector<CommandBufferHandle> graphicsCommandBuffers_;
		std::vector<FenceHandle> graphicsFences_;
		std::vector<CommandBufferHandle> computeCommandBuffers_;
		std::vector<FenceHandle> computeFences_;
		std::vector<SemaphoreHandle> imageAcquireSemaphores_;
		std::vector<SemaphoreHandle> renderCompleteSemaphores_;

	public:
		Swapchain(const Device& device, uint32_t width, uint32_t height);
		~Swapchain() = default;

		void Recreate(uint32_t width, uint32_t height);
		CommandBufferHandle& GetCurrentCommandBuffer();
		void WaitFrame();
		void Present();

		const std::vector<vk::Image>& GetSwapchainImages() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		vk::Extent2D GetExtent2D() const;
		vk::Format GetSurfaceFormat() const;
		vk::Image GetCurrentImage() const;
		uint32_t GetInflightCount() const;
		uint32_t GetCurrentInflightIndex() const;
		uint32_t GetImageIndex() const;
		uint32_t GetMinImageCount() const;
		SemaphoreHandle GetImageAcquireSemaphore() const;
		SemaphoreHandle GetRenderCompleteSemaphore() const;
		FenceHandle GetCurrentFence() const;
	};
}