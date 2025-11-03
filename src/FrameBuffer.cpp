#include "FrameBuffer.hpp"

#include "Device.hpp"
#include "Image.hpp"
#include "RenderPass.hpp"
#include "Swapchain.hpp"

using namespace std;

namespace sqrp
{
	FrameBuffer::FrameBuffer(const Device& device, RenderPassHandle pRenderPass, SwapchainHandle pSwapchain, std::vector<ImageHandle> depthImages)
		: pDevice_(&device), pRenderPass_(pRenderPass)
	{
		pSwapchain_ = pSwapchain;
		inflightCount_ = pSwapchain->GetInflightCount();
		swapchainImageViews_ = vector<vk::UniqueImageView>();
		width_ = pSwapchain->GetWidth();
		height_ = pSwapchain->GetHeight();

		// Create swapchain image views
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

		if (depthImages.size() > 0) {
			attachmentImages_[0] = vector<ImageHandle>();
			for (size_t j = 0; j < pSwapchain->GetInflightCount(); j++) {
				attachmentImages_[0].push_back(
					depthImages[j]
				);
			}
		}

		framebuffers_.resize(pSwapchain->GetInflightCount());
		for (int i = 0; i < pSwapchain->GetInflightCount(); i++) {
			vector<vk::ImageView> attachments;
			if (pSwapchain_.has_value()) {
				attachments.push_back(swapchainImageViews_.value()[i].get());
			}
			if (depthImages.size() > 0) {
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

	FrameBuffer::FrameBuffer(const Device& device, RenderPassHandle pRenderPass, std::vector<std::vector<ImageHandle>> attachmentImages, uint32_t width, uint32_t height, int inflightCount, SwapchainHandle pSwapchain)
		: pDevice_(&device), pRenderPass_(pRenderPass), attachmentImages_(attachmentImages), inflightCount_(inflightCount), width_(width), height_(height)
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
	
		//for (int i = 0; i < pImages.size(); i++) {
		//	attachmentImages_.push_back(vector<ImageHandle>());
		//	for (size_t j = 0; j < inflightCount_; j++) {
		//		if (frameBufferInfos_[i].pImage->GetUsage() & vk::ImageUsageFlagBits::eColorAttachment) {
		//			//cout << "Creating color attachment image: " << frameBufferInfos_[i].debugName << endl;
		//			attachmentImages_[i].push_back(frameBufferInfos_[i].pImage);
		//			/*attachmentImages_[i].push_back(
		//				pDevice_->CreateImage(
		//					frameBufferInfos_[i].debugName + to_string(j),
		//					vk::Extent3D{ width_, height_, 1 },
		//					vk::ImageType::e2D,
		//					frameBufferInfos_[i].imageUsage,
		//					frameBufferInfos_[i].format,
		//					vk::ImageLayout::eUndefined,
		//					vk::ImageAspectFlagBits::eColor,
		//					1,
		//					1,
		//					vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
		//					vk::SamplerCreateInfo{}
		//					.setMagFilter(vk::Filter::eLinear)
		//					.setMinFilter(vk::Filter::eLinear)
		//					.setMipmapMode(vk::SamplerMipmapMode::eNearest)
		//					.setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
		//					.setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
		//					.setAddressModeW(vk::SamplerAddressMode::eClampToEdge)
		//					.setMipLodBias(0.0f)
		//					.setAnisotropyEnable(VK_FALSE)
		//					.setMaxAnisotropy(1.0f)
		//					.setCompareEnable(VK_FALSE)
		//					.setCompareOp(vk::CompareOp::eAlways)
		//					.setMinLod(0.0f)
		//					.setMaxLod(0.0f)
		//					.setBorderColor(vk::BorderColor::eIntOpaqueWhite)
		//					.setUnnormalizedCoordinates(VK_FALSE)
		//				));*/
		//		}
		//		else if (frameBufferInfos_[i].pImage->GetUsage() & vk::ImageUsageFlagBits::eDepthStencilAttachment) {
		//			//cout << "Creating depth attachment image: " << frameBufferInfos_[i].debugName << endl;
		//			attachmentImages_[i].push_back(frameBufferInfos_[i].pImage);
		//			/*attachmentImages_[i].push_back(
		//				pDevice_->CreateImage(
		//					frameBufferInfos_[i].debugName + to_string(j),
		//					vk::Extent3D{ width_, height_, 1 },
		//					vk::ImageType::e2D,
		//					frameBufferInfos_[i].imageUsage,
		//					frameBufferInfos_[i].format,
		//					vk::ImageLayout::eUndefined,
		//					vk::ImageAspectFlagBits::eDepth,
		//					1,
		//					1,
		//					vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
		//					vk::SamplerCreateInfo{}
		//					.setMagFilter(vk::Filter::eNearest)
		//					.setMinFilter(vk::Filter::eNearest)
		//					.setMipmapMode(vk::SamplerMipmapMode::eNearest)
		//					.setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
		//					.setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
		//					.setAddressModeW(vk::SamplerAddressMode::eClampToEdge)
		//					.setMipLodBias(0.0f)
		//					.setAnisotropyEnable(VK_FALSE)
		//					.setMaxAnisotropy(1.0f)
		//					.setCompareEnable(VK_FALSE)
		//					.setCompareOp(vk::CompareOp::eAlways)
		//					.setMinLod(0.0f)
		//					.setMaxLod(0.0f)
		//					.setBorderColor(vk::BorderColor::eIntOpaqueWhite)
		//					.setUnnormalizedCoordinates(VK_FALSE)
		//				)
		//			);*/
		//		}
		//	}
		//}

		framebuffers_.resize(inflightCount_);
		for (int i = 0; i < inflightCount_; i++) {
			vector<vk::ImageView> attachments;
			if (pSwapchain_.has_value()) {
				attachments.push_back(swapchainImageViews_.value()[i].get());
			}
			for (int j = 0; j < attachmentImages_.size(); j++) {
				attachments.push_back(attachmentImages_[j][i]->GetImageView());
			}
			cout << "Framebuffer attachments: " << attachments.size() << endl;
			vk::FramebufferCreateInfo framebufferInfo = {};
			framebufferInfo
				.setRenderPass(pRenderPass_->GetRenderPass())
				.setAttachmentCount(static_cast<uint32_t>(attachments.size()))
				.setPAttachments(attachments.data())
				.setWidth(width_)
				.setHeight(height_)
				.setLayers(1);
			framebuffers_[i] = pDevice_->GetDevice().createFramebufferUnique(framebufferInfo);
		}
	}

	void FrameBuffer::Recreate(uint32_t width, uint32_t height)
	{
		width_ = width;
		height_ = height;

		framebuffers_.clear();
		framebuffers_.resize(inflightCount_);

		swapchainImageViews_.reset();
		//attachmentImages_.clear();

		if (pSwapchain_.has_value()) {
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

		//for (int i = 0; i < frameBufferInfos_.size(); i++) {
		//	attachmentImages_[i] = vector<ImageHandle>();
		//	for (size_t j = 0; j < inflightCount_; j++) {
		//		if (frameBufferInfos_[i].pImage->GetUsage() & vk::ImageUsageFlagBits::eColorAttachment) {
		//			//cout << "Creating color attachment image: " << frameBufferInfos_[i].debugName << endl;
		//			/*attachmentImages_[i].push_back(
		//				pDevice_->CreateImage(
		//					frameBufferInfos_[i].debugName + to_string(j),
		//					vk::Extent3D{ width_, height_, 1 },
		//					vk::ImageType::e2D,
		//					frameBufferInfos_[i].imageUsage,
		//					frameBufferInfos_[i].format,
		//					vk::ImageLayout::eUndefined,
		//					vk::ImageAspectFlagBits::eColor,
		//					1,
		//					1,
		//					vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
		//					vk::SamplerCreateInfo{}
		//					.setMagFilter(vk::Filter::eLinear)
		//					.setMinFilter(vk::Filter::eLinear)
		//					.setMipmapMode(vk::SamplerMipmapMode::eNearest)
		//					.setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
		//					.setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
		//					.setAddressModeW(vk::SamplerAddressMode::eClampToEdge)
		//					.setMipLodBias(0.0f)
		//					.setAnisotropyEnable(VK_FALSE)
		//					.setMaxAnisotropy(1.0f)
		//					.setCompareEnable(VK_FALSE)
		//					.setCompareOp(vk::CompareOp::eAlways)
		//					.setMinLod(0.0f)
		//					.setMaxLod(0.0f)
		//					.setBorderColor(vk::BorderColor::eIntOpaqueWhite)
		//					.setUnnormalizedCoordinates(VK_FALSE)
		//				));*/
		//			attachmentImages_[i].push_back(frameBufferInfos_[i].pImage);
		//		}
		//		else if (frameBufferInfos_[i].pImage->GetUsage() & vk::ImageUsageFlagBits::eDepthStencilAttachment) {
		//			//cout << "Creating depth attachment image: " << frameBufferInfos_[i].debugName << endl;
		//			/*attachmentImages_[i].push_back(
		//				pDevice_->CreateImage(
		//					frameBufferInfos_[i].debugName + to_string(j),
		//					vk::Extent3D{ width_, height_, 1 },
		//					vk::ImageType::e2D,
		//					frameBufferInfos_[i].imageUsage,
		//					frameBufferInfos_[i].format,
		//					vk::ImageLayout::eUndefined,
		//					vk::ImageAspectFlagBits::eDepth,
		//					1,
		//					1,
		//					vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
		//					vk::SamplerCreateInfo{}
		//					.setMagFilter(vk::Filter::eNearest)
		//					.setMinFilter(vk::Filter::eNearest)
		//					.setMipmapMode(vk::SamplerMipmapMode::eNearest)
		//					.setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
		//					.setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
		//					.setAddressModeW(vk::SamplerAddressMode::eClampToEdge)
		//					.setMipLodBias(0.0f)
		//					.setAnisotropyEnable(VK_FALSE)
		//					.setMaxAnisotropy(1.0f)
		//					.setCompareEnable(VK_FALSE)
		//					.setCompareOp(vk::CompareOp::eAlways)
		//					.setMinLod(0.0f)
		//					.setMaxLod(0.0f)
		//					.setBorderColor(vk::BorderColor::eIntOpaqueWhite)
		//					.setUnnormalizedCoordinates(VK_FALSE)
		//				)
		//			);*/
		//			attachmentImages_[i].push_back(frameBufferInfos_[i].pImage);
		//		}
		//	}
		//}

		for (int i = 0; i < inflightCount_; i++) {
			vector<vk::ImageView> attachments;
			if (pSwapchain_.has_value()) {
				attachments.push_back(swapchainImageViews_.value()[i].get());
				//cout << "Framebuffer has swapchain image view" << endl;
			}
			for (int j = 0; j < attachmentImages_.size(); j++) {
				attachments.push_back(attachmentImages_[j][i]->GetImageView());
				//cout << "Framebuffer has attachment image view: " << frameBufferInfos_[j].debugName << endl;
			}
			//cout << "Framebuffer attachments: " << attachments.size() << endl;
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

	//ImageHandle FrameBuffer::GetAttachmentImage(int index, int inflightIndex) const
	//{
	//	auto it = attachmentImages_.find(index);
	//	if (it == attachmentImages_.end()) {
	//		throw std::runtime_error("No such attachment buffer");
	//	}
	//	if (inflightIndex < 0 || inflightIndex >= it->second.size()) {
	//		throw std::runtime_error("Inflight index out of bounds");
	//	}
	//	return it->second[inflightIndex]; // check bounds
	//}

	uint32_t FrameBuffer::GetWidth() const
	{
		return width_;
	}

	uint32_t FrameBuffer::GetHeight() const
	{
		return height_;
	}
}