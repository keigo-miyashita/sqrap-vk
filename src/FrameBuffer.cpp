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
		inflightCount_ = pSwapchain->GetInflightCount();
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
			frameBufferInfos_.push_back({ vk::Format::eD32Sfloat, vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled, "DepthAttachment" });
		}

		framebuffers_.resize(pSwapchain->GetInflightCount());
		for (int i = 0; i < pSwapchain->GetInflightCount(); i++) {
			vector<vk::ImageView> attachments;
			if (pSwapchain_.has_value()) {
				attachments.push_back(swapchainImageViews_.value()[i].get());
				//frameBufferInfos_.push_back({ pSwapchain->GetSurfaceFormat(), vk::ImageUsageFlagBits::eColorAttachment, "SwapchainColorAttachment" });
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
		: pDevice_(&device), pRenderPass_(pRenderPass), frameBufferInfos_(frameBufferInfos), inflightCount_(infligtCount)
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
				if (frameBufferInfos_[i].imageUsage & vk::ImageUsageFlagBits::eDepthStencilAttachment) {
					cout << "Creating depth attachment image: " << frameBufferInfos_[i].debugName << endl;
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
				else if (frameBufferInfos_[i].imageUsage & vk::ImageUsageFlagBits::eColorAttachment) {
					cout << "Creating color attachment image: " << frameBufferInfos_[i].debugName << endl;
					attachmentImages_[i].push_back(
						pDevice_->CreateImage(
							frameBufferInfos[i].debugName + to_string(j),
							vk::Extent3D{ width, height, 1 },
							vk::ImageType::e2D,
							frameBufferInfos[i].imageUsage,
							frameBufferInfos[i].format,
							vk::ImageLayout::eUndefined,
							(frameBufferInfos[i].imageUsage & vk::ImageUsageFlagBits::eDepthStencilAttachment) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor,
							1,
							1,
							vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
							vk::SamplerCreateInfo{}
								.setMagFilter(vk::Filter::eLinear)
								.setMinFilter(vk::Filter::eLinear)
								.setMipmapMode(vk::SamplerMipmapMode::eNearest)
								.setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
								.setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
								.setAddressModeW(vk::SamplerAddressMode::eClampToEdge)
								.setMipLodBias(0.0f)
								.setAnisotropyEnable(VK_FALSE)
								.setMaxAnisotropy(1.0f)
								.setCompareEnable(VK_FALSE)
								.setCompareOp(vk::CompareOp::eAlways)
								.setMinLod(0.0f)
								.setMaxLod(0.0f)
								.setBorderColor(vk::BorderColor::eIntOpaqueWhite)
								.setUnnormalizedCoordinates(VK_FALSE)
						));
				}
				/*attachmentImages_[i].push_back(
					pDevice_->CreateImage(
						frameBufferInfos[i].debugName + to_string(j),
						vk::Extent3D{ width, height, 1 },
						vk::ImageType::e2D,
						frameBufferInfos[i].imageUsage,
						frameBufferInfos[i].format,
						vk::ImageLayout::eUndefined,
						(frameBufferInfos[i].imageUsage & vk::ImageUsageFlagBits::eDepthStencilAttachment) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor
					)
				);*/
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

	void FrameBuffer::Recreate(uint32_t width, uint32_t height)
	{
		framebuffers_.clear();
		framebuffers_.resize(inflightCount_);

		swapchainImageViews_.reset();
		attachmentImages_.clear();

		if (pSwapchain_.has_value()) {
			//pSwapchain_ = pSwapchain;
			swapchainImageViews_ = vector<vk::UniqueImageView>();
			for (const auto& swapchainImage : pSwapchain_.value()->GetSwapchainImages()) {
				swapchainImageViews_->push_back(
					pDevice_->GetDevice().createImageViewUnique(
						vk::ImageViewCreateInfo()
						.setImage(swapchainImage)
						.setViewType(vk::ImageViewType::e2D)
						.setFormat(pSwapchain_.value()->GetSurfaceFormat())
						.setComponents({ vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG,
									vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA })
						.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }))
				);
			}
		}

		for (int i = 0; i < frameBufferInfos_.size(); i++) {
			attachmentImages_[i] = vector<ImageHandle>();
			for (size_t j = 0; j < inflightCount_; j++) {
				attachmentImages_[i].push_back(
					pDevice_->CreateImage(
						frameBufferInfos_[i].debugName + to_string(j),
						vk::Extent3D{ width, height, 1 },
						vk::ImageType::e2D,
						frameBufferInfos_[i].imageUsage,
						frameBufferInfos_[i].format,
						vk::ImageLayout::eUndefined,
						(frameBufferInfos_[i].imageUsage & vk::ImageUsageFlagBits::eDepthStencilAttachment) ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor
					)
				);
			}
		}

		/*framebuffers_.resize(inflightCount_);*/
		for (int i = 0; i < inflightCount_; i++) {
			vector<vk::ImageView> attachments;
			if (pSwapchain_.has_value()) {
				attachments.push_back(swapchainImageViews_.value()[i].get());
				cout << "Framebuffer has swapchain image view" << endl;
			}
			for (int j = 0; j < frameBufferInfos_.size(); j++) {
				attachments.push_back(attachmentImages_[j][i]->GetImageView());
				cout << "Framebuffer has attachment image view: " << frameBufferInfos_[j].debugName << endl;
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