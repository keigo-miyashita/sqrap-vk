#include "RenderPass.hpp"

#include "Device.hpp"

using namespace std;

namespace sqrp
{
	RenderPass::RenderPass(
		const Device& device
	)
		: pDevice_(&device)
	{
        // カラーアタッチメント
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.format = colorFormat;
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear; // 初期化時にクリア
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore; // 書き戻す
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        // 深度アタッチメント
        vk::AttachmentDescription depthAttachment{};
        depthAttachment.format = depthFormat;
        depthAttachment.samples = vk::SampleCountFlagBits::e1;
        depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
        depthAttachment.storeOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
        depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        // アタッチメント参照
        vk::AttachmentReference colorRef{};
        colorRef.attachment = 0;
        colorRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::AttachmentReference depthRef{};
        depthRef.attachment = 1;
        depthRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        // サブパス
        vk::SubpassDescription subpass{};
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorRef;
        subpass.pDepthStencilAttachment = &depthRef;

        // RenderPass 作成
        std::array<vk::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        renderPass_ = pDevice_->GetDevice().createRenderPassUnique(renderPassInfo);
	}

    vk::RenderPass RenderPass::GetRenderPass() const
    {
        return renderPass_.get();
    }
}