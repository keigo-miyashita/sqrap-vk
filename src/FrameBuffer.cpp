#include "FrameBuffer.hpp"

#include "Device.hpp"
#include "Image.hpp"
#include "RenderPass.hpp"
#include "Swapchain.hpp"

using namespace std;

namespace sqrp
{
	FrameBuffer::FrameBuffer(const Device& device, RenderPassHandle pRenderPass, SwapchainHandle pSwapchain, bool useDepth)
		: pDevice_(&device), pRenderPass_(pRenderPass)
	{
		pSwapchain_ = pSwapchain;
		swapchainImageViews_ = vector<vk::UniqueImageView>();
		for (const auto& swapchainImage : pSwapchain->GetSwapchainImages()) {
			swapchainImageViews_->push_back(
				pDevice_->GetDevice().createImageViewUnique(
					vk::ImageViewCreateInfo()
					.setImage(swapchainImage)
					.setViewType(vk::ImageViewType::e2D)
					.setFormat(pSwapchain->GetSurfaceFormat())
					.setComponents({ vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG,
								vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA })
					.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }))
			);
		}

		if (useDepth) {
			attachmentImages_[0] = vector<ImageHandle>();
			for (size_t j = 0; j < pSwapchain->GetInflightCount(); j++) {
				attachmentImages_[0].push_back(
					pDevice_->CreateImage(
						vk::Extent3D{ pSwapchain->GetWidth(), pSwapchain->GetHeight(), 1 },
						vk::ImageType::e2D,
						vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled,
						vk::Format::eD32Sfloat,
						vk::ImageLayout::eUndefined,
						vk::ImageAspectFlagBits::eDepth
					)
				);
			}
		}

		framebuffers_.resize(pSwapchain->GetInflightCount());
		for (int i = 0; i < pSwapchain->GetInflightCount(); i++) {
			vector<vk::ImageView> attachments;
			if (pSwapchain_.has_value()) {
				attachments.push_back(swapchainImageViews_.value()[i].get());
			}
			if (useDepth) {
				attachments.push_back(attachmentImages_[0][i]->GetImageView());
			}
			vk::FramebufferCreateInfo framebufferInfo = {};
			framebufferInfo
				.setRenderPass(pRenderPass_->GetRenderPass())
				.setAttachmentCount(static_cast<uint32_t>(attachments.size()))
				.setPAttachments(attachments.data())
				.setWidth(pSwapchain->GetWidth())
				.setHeight(pSwapchain->GetHeight())
				.setLayers(1);
			framebuffers_[i] = pDevice_->GetDevice().createFramebufferUnique(framebufferInfo);
		}
	}

	FrameBuffer::FrameBuffer(const Device& device, RenderPassHandle pRenderPass, std::vector<FrameBufferInfo> frameBufferInfos, uint32_t width, uint32_t height, int infligtCount, SwapchainHandle pSwapchain)
		: pDevice_(&device), pRenderPass_(pRenderPass)
	{
		if (pSwapchain != nullptr) {
			pSwapchain_ = pSwapchain;
			swapchainImageViews_ = vector<vk::UniqueImageView>();
			for (const auto& swapchainImage : pSwapchain->GetSwapchainImages()) {
				swapchainImageViews_->push_back(
					pDevice_->GetDevice().createImageViewUnique(
						vk::ImageViewCreateInfo()
						.setImage(swapchainImage)
						.setViewType(vk::ImageViewType::e2D)
						.setFormat(pSwapchain->GetSurfaceFormat())
						.setComponents({ vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG,
									vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA })
						.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }))
				);
			}
		}
	
		for (int i = 0; i < frameBufferInfos.size(); i++) {
			attachmentImages_[i] = vector<ImageHandle>();
			for (size_t j = 0; j < infligtCount; j++) {
				attachmentImages_[i].push_back(
					pDevice_->CreateImage(
						frameBufferInfos[i].debugName + to_string(j),
						vk::Extent3D{ width, height, 1 },
						vk::ImageType::e2D,
						frameBufferInfos[i].imageUsage,
						frameBufferInfos[i].format,
						vk::ImageLayout::eUndefined,
						(frameBufferInfos[i].imageUsage & vk::ImageUsageFlagBits::eDepthStencilAttachment) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor
					)
				);
			}
		}

		framebuffers_.resize(infligtCount);
		for (int i = 0; i < infligtCount; i++) {
			vector<vk::ImageView> attachments;
			if (pSwapchain_.has_value()) {
				attachments.push_back(swapchainImageViews_.value()[i].get());
			}
			for (int j = 0; j < frameBufferInfos.size(); j++) {
				attachments.push_back(attachmentImages_[j][i]->GetImageView());
			}
			cout << "Framebuffer attachments: " << attachments.size() << endl;
			vk::FramebufferCreateInfo framebufferInfo = {};
			framebufferInfo
				.setRenderPass(pRenderPass_->GetRenderPass())
				.setAttachmentCount(static_cast<uint32_t>(attachments.size()))
				.setPAttachments(attachments.data())
				.setWidth(width)
				.setHeight(height)
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
		if (pSwapchain_ == std::nullopt) {
			throw std::runtime_error("This framebuffer is not associated with a swapchain");
		}
		else {
			return pSwapchain_.value();
		}
	}

	ImageHandle FrameBuffer::GetAttachmentImage(int index, int inflightIndex) const
	{
		auto it = attachmentImages_.find(index);
		if (it == attachmentImages_.end()) {
			throw std::runtime_error("No such attachment buffer");
		}
		if (inflightIndex < 0 || inflightIndex >= it->second.size()) {
			throw std::runtime_error("Inflight index out of bounds");
		}
		return it->second[inflightIndex]; // check bounds
	}
}