#include "Swapchain.hpp"

#include "CommandBuffer.hpp"
#include "Device.hpp"
#include "Fence.hpp"
#include "Semaphore.hpp"

using namespace std;

namespace sqrp
{
	Swapchain::Swapchain(const Device& device, uint32_t width, uint32_t height)
		: pDevice_(&device), width_(width), height_(height)
	{
		capabilities_ = pDevice_->GetPhysicalDevice().getSurfaceCapabilitiesKHR(pDevice_->GetSurface());
		if (capabilities_.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
			throw std::runtime_error("Swapchain capabilities error");
		}

		auto formats = pDevice_->GetPhysicalDevice().getSurfaceFormatsKHR(pDevice_->GetSurface());
		surfaceFormat_ = formats[0];
		for (const auto& format : formats) {
			if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				surfaceFormat_ = format;
			}
		}

		auto presentModes = pDevice_->GetPhysicalDevice().getSurfacePresentModesKHR(pDevice_->GetSurface());
		presentMode_ = presentModes[0];
		for (const auto& presentMode : presentModes) {
			if (presentMode == vk::PresentModeKHR::eMailbox/*Vsync On, has almost 3 buffer, low laytency, no tearing*/) {
				presentMode_ = presentMode;
			}
		}

		// Remove duplicate queue family indices
		set<uint32_t> uniqueQueueFamilyIndices;
		/*= {
			pDevice_->GetGraphicsQueueFamilyIndex(),
			pDevice_->GetPresentQueueFamilyIndex()
		};*/
		for (const auto& element : pDevice_->GetQueueContexts()) {
			uniqueQueueFamilyIndices.insert(element.second.queueFamilyIndex);
		}
		vector<uint32_t> requiredQueueFamilyIndices(uniqueQueueFamilyIndices.begin(), uniqueQueueFamilyIndices.end());

		swapchain_ = pDevice_->GetDevice().createSwapchainKHRUnique(
			vk::SwapchainCreateInfoKHR()
			.setSurface(pDevice_->GetSurface())
			.setMinImageCount(minImageCount_)
			.setImageFormat(surfaceFormat_.format)
			.setImageColorSpace(surfaceFormat_.colorSpace)
			.setImageExtent({ width_, height_ })
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst) // RenderTarget | Copy Destination
			.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity) // NOTE : This specify no rotation if you support rotation for mobile, fix it
			.setPresentMode(presentMode_)
			.setClipped(true)
			.setQueueFamilyIndices(requiredQueueFamilyIndices)
		);

		swapchainImages_ = pDevice_->GetDevice().getSwapchainImagesKHR(swapchain_.get());

		for (const auto& [flag, context] : pDevice_->GetQueueContexts()) {
			if (flag == QueueContextType::General || flag == QueueContextType::Graphics) {
				graphicsCommandBuffers_.resize(inflightCount_);
				graphicsFences_.resize(inflightCount_);
				for (int i = 0; i < inflightCount_; i++) {
					graphicsCommandBuffers_[i] = pDevice_->CreateCommandBuffer(flag);
					graphicsFences_[i] = pDevice_->CreateFence();
				}
			}
			else if (flag == QueueContextType::Compute) {
				computeCommandBuffers_.resize(inflightCount_);
				computeFences_.resize(inflightCount_);
				for (int i = 0; i < inflightCount_; i++) {
					computeCommandBuffers_[i] = pDevice_->CreateCommandBuffer(flag);
					computeFences_[i] = pDevice_->CreateFence();
				}
			}
		}

		imageAcquireSemaphores_.resize(inflightCount_);
		renderCompleteSemaphores_.resize(inflightCount_);
		for (int i = 0; i < inflightCount_; i++) {
			imageAcquireSemaphores_[i] = pDevice_->CreateSemaphore("ImageAcquireSemaphore" + to_string(i));
			renderCompleteSemaphores_[i] = pDevice_->CreateSemaphore("RenderCompleteSemaphore" + to_string(i));
		}
	}

	void Swapchain::Recreate(uint32_t width, uint32_t height)
	{
		width_ = width;
		height_ = height;
		inflightIndex_ = 0;
		imageIndex_ = 0;

		swapchainImages_.clear();
		graphicsCommandBuffers_.clear();
		graphicsFences_.clear();
		computeCommandBuffers_.clear();
		computeFences_.clear();
		imageAcquireSemaphores_.clear();
		renderCompleteSemaphores_.clear();

		swapchain_.reset();

		capabilities_ = pDevice_->GetPhysicalDevice().getSurfaceCapabilitiesKHR(pDevice_->GetSurface());
		if (capabilities_.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
			throw std::runtime_error("Swapchain capabilities error");
		}

		auto formats = pDevice_->GetPhysicalDevice().getSurfaceFormatsKHR(pDevice_->GetSurface());
		surfaceFormat_ = formats[0];
		for (const auto& format : formats) {
			if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				surfaceFormat_ = format;
			}
		}

		auto presentModes = pDevice_->GetPhysicalDevice().getSurfacePresentModesKHR(pDevice_->GetSurface());
		presentMode_ = presentModes[0];
		for (const auto& presentMode : presentModes) {
			if (presentMode == vk::PresentModeKHR::eMailbox/*Vsync On, has almost 3 buffer, low laytency, no tearing*/) {
				presentMode_ = presentMode;
			}
		}

		// Remove duplicate queue family indices
		set<uint32_t> uniqueQueueFamilyIndices;
		/*= {
			pDevice_->GetGraphicsQueueFamilyIndex(),
			pDevice_->GetPresentQueueFamilyIndex()
		};*/
		for (const auto& element : pDevice_->GetQueueContexts()) {
			uniqueQueueFamilyIndices.insert(element.second.queueFamilyIndex);
		}
		vector<uint32_t> requiredQueueFamilyIndices(uniqueQueueFamilyIndices.begin(), uniqueQueueFamilyIndices.end());

		swapchain_ = pDevice_->GetDevice().createSwapchainKHRUnique(
			vk::SwapchainCreateInfoKHR()
			.setSurface(pDevice_->GetSurface())
			.setMinImageCount(minImageCount_)
			.setImageFormat(surfaceFormat_.format)
			.setImageColorSpace(surfaceFormat_.colorSpace)
			.setImageExtent({ width_, height_ })
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst) // RenderTarget | Copy Destination
			.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity) // NOTE : This specify no rotation if you support rotation for mobile, fix it
			.setPresentMode(presentMode_)
			.setClipped(true)
			.setQueueFamilyIndices(requiredQueueFamilyIndices)
		);

		swapchainImages_ = pDevice_->GetDevice().getSwapchainImagesKHR(swapchain_.get());

		for (const auto& [flag, context] : pDevice_->GetQueueContexts()) {
			if (flag == QueueContextType::General || flag == QueueContextType::Graphics) {
				graphicsCommandBuffers_.resize(inflightCount_);
				graphicsFences_.resize(inflightCount_);
				for (int i = 0; i < inflightCount_; i++) {
					graphicsCommandBuffers_[i] = pDevice_->CreateCommandBuffer(flag);
					graphicsFences_[i] = pDevice_->CreateFence();
				}
			}
			else if (flag == QueueContextType::Compute) {
				computeCommandBuffers_.resize(inflightCount_);
				computeFences_.resize(inflightCount_);
				for (int i = 0; i < inflightCount_; i++) {
					computeCommandBuffers_[i] = pDevice_->CreateCommandBuffer(flag);
					computeFences_[i] = pDevice_->CreateFence();
				}
			}
		}

		imageAcquireSemaphores_.resize(inflightCount_);
		renderCompleteSemaphores_.resize(inflightCount_);
		for (int i = 0; i < inflightCount_; i++) {
			imageAcquireSemaphores_[i] = pDevice_->CreateSemaphore("ImageAcquireSemaphore" + to_string(i));
			renderCompleteSemaphores_[i] = pDevice_->CreateSemaphore("RenderCompleteSemaphore" + to_string(i));
		}
	}

	CommandBufferHandle& Swapchain::GetCurrentCommandBuffer()
	{
		return graphicsCommandBuffers_[inflightIndex_];
	}

	void Swapchain::WaitFrame()
	{
		graphicsFences_[inflightIndex_]->Wait();

		auto result = pDevice_->GetDevice().acquireNextImageKHR(swapchain_.get(), std::numeric_limits<uint64_t>::max(), imageAcquireSemaphores_[inflightIndex_]->GetSemaphore(), nullptr);

		imageIndex_ = result.value;

		graphicsFences_[inflightIndex_]->Reset();
	}

	void Swapchain::Present()
	{
		vk::PresentInfoKHR presentInfo;
		presentInfo.setSwapchains(swapchain_.get());
		presentInfo.setImageIndices(imageIndex_);
		vk::Semaphore semaphore = renderCompleteSemaphores_[inflightIndex_]->GetSemaphore();
		presentInfo.setWaitSemaphores(semaphore);
		if (pDevice_->GetQueue(QueueContextType::General).presentKHR(presentInfo) != vk::Result::eSuccess) {
			return;
		}
		inflightIndex_++;
		inflightIndex_ %= inflightCount_;
	}

	const std::vector<vk::Image>& Swapchain::GetSwapchainImages() const
	{
		return swapchainImages_;
	}

	uint32_t Swapchain::GetWidth() const
	{
		return width_;
	}

	uint32_t Swapchain::GetHeight() const
	{
		return height_;
	}

	vk::Extent2D Swapchain::GetExtent2D() const
	{
		return vk::Extent2D{ width_, height_ };
	}

	vk::Format Swapchain::GetSurfaceFormat() const
	{
		return surfaceFormat_.format;
	}

	uint32_t Swapchain::GetInflightCount() const
	{
		return inflightCount_;
	}

	uint32_t Swapchain::GetCurrentInflightIndex() const
	{
		return inflightIndex_;
	}

	uint32_t Swapchain::GetImageIndex() const
	{
		return imageIndex_;
	}

	uint32_t Swapchain::GetMinImageCount() const
	{
		return minImageCount_;
	}

	SemaphoreHandle Swapchain::GetImageAcquireSemaphore() const
	{
		return imageAcquireSemaphores_[inflightIndex_];
	}

	SemaphoreHandle Swapchain::GetRenderCompleteSemaphore() const
	{
		return renderCompleteSemaphores_[inflightIndex_];
	}

	FenceHandle Swapchain::GetCurrentFence() const
	{
		return graphicsFences_[inflightIndex_];
	}

}