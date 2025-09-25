#include "FrameBuffer.hpp"

#include "Device.hpp"
#include "Image.hpp"
#include "Swapchain.hpp"

using namespace std;

namespace sqrp
{
	FrameBuffer::FrameBuffer(const Device& device, SwapchainHandle pSwapchain, int numAttachmentBuffers)
		: pDevice_(&device), pSwapchain_(pSwapchain)
	{
		for (const auto& swapchainImage : pSwapchain_->GetSwapchainImages()) {
			swapchainImageViews_.push_back(
				pDevice_->GetDevice().createImageViewUnique(
					vk::ImageViewCreateInfo()
					.setImage(swapchainImage)
					.setViewType(vk::ImageViewType::e2D)
					.setFormat(pSwapchain_->GetSurfaceFormat())
					.setComponents({ vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG,
								vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA })
					.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }))
			);
		}

		for (int i = 0; i < numAttachmentBuffers; i++) {
			attachmentBuffers_[i] = vector<ImageHandle>();
			for (size_t j = 0; j < pSwapchain_->GetInflightCount(); j++) {
				attachmentBuffers_[i].push_back(
					pDevice_->CreateImage(
						vk::Extent3D{ pSwapchain_->GetWidth(), pSwapchain_->GetHeight(), 1 },
						vk::ImageType::e2D,
						vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
						vk::Format::eR8G8B8A8Srgb
					)
				);
			}
		}


	}


}