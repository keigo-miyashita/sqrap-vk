#include "Swapchain.hpp"

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
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				surfaceFormat_ = format;
			}
		}

		auto presentModes = pDevice_->GetPhysicalDevice().getSurfacePresentModesKHR(pDevice_->GetSurface());
		presentMode_ = presentModes[0];
		for (const auto& presentMode : presentModes) {
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR/*Vsync On, has almost 3 buffer, low laytency, no tearing*/) {
				presentMode_ = presentMode;
			}
		}

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
			.setQueueFamilyIndices(pDevice_->GetQueueFamilyIndex());

		swapchainImages_ = pDevice_->GetDevice().getSwapchainImagesKHR(swapchain_.get());

		for (auto& swapchainImage : swapchainImages_) {
			swapchainImageViews_.push_back(
				vk::ImageViewCreateInfo()
					.setImage(image)
					.setViewType(vk::ImageViewType::e2D)
					.setFormat(surfaceFormat_)
					.setComponents({ vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG,
								vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA })
					.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 })));
			)
		}
	}


}