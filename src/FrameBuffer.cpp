#include "FrameBuffer.hpp"

#include "Device.hpp"
#include "Image.hpp"
#include "RenderPass.hpp"
#include "Swapchain.hpp"

using namespace std;

namespace sqrp
{
	FrameBuffer::FrameBuffer(const Device& device, SwapchainHandle pSwapchain, RenderPassHandle pRenderPass, int numAttachmentBuffers)
		: pDevice_(&device), pRenderPass_(pRenderPass), pSwapchain_(pSwapchain)
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
						vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled,
						vk::Format::eD32Sfloat,
						vk::ImageLayout::eUndefined,
						vk::ImageAspectFlagBits::eDepth
					)
				);
			}
		}

		framebuffers_.resize(pSwapchain_->GetInflightCount());
		for (int i = 0; i < pSwapchain_->GetInflightCount(); i++) {
			vector<vk::ImageView> attachments;
			attachments.push_back(swapchainImageViews_[i].get());
			for (int j = 0; j < numAttachmentBuffers; j++) {
				attachments.push_back(attachmentBuffers_[j][i]->GetImageView());
			}
			vk::FramebufferCreateInfo framebufferInfo = {};
			framebufferInfo
				.setRenderPass(pRenderPass_->GetRenderPass())
				.setAttachmentCount(static_cast<uint32_t>(attachments.size()))
				.setPAttachments(attachments.data())
				.setWidth(pSwapchain_->GetWidth())
				.setHeight(pSwapchain_->GetHeight())
				.setLayers(1);
			framebuffers_[i] = pDevice_->GetDevice().createFramebufferUnique(framebufferInfo);
		}
	}

	vk::Framebuffer FrameBuffer::GetFrameBuffer(int index) const
	{
		return framebuffers_[index].get();
	}

	SwapchainHandle FrameBuffer::GetSwapchain() const
	{
		return pSwapchain_;
	}
}