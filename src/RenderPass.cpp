#include "RenderPass.hpp"

#include "Device.hpp"
#include "Swapchain.hpp"

using namespace std;

namespace sqrp
{
	RenderPass::RenderPass(
        const Device& device,
        std::string name,
        SwapchainHandle pSwapchain,
        bool depth
	)
		: pDevice_(&device)
	{
        numColorAttachments_ = 1;
		numDepthAttachments_ = 1;

		// Color Attachment for swapchain
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.format = pSwapchain->GetSurfaceFormat();
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear; // èâä˙âªéûÇ…ÉNÉäÉA
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore; // èëÇ´ñﬂÇ∑
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentReference colorRef{};
        colorRef.attachment = 0;
        colorRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

		// Depth Attachment
        vk::AttachmentReference depthRef{};
        vk::AttachmentDescription depthAttachment{};
        if (depth) {
            depthAttachment.format = vk::Format::eD32Sfloat;
            depthAttachment.samples = vk::SampleCountFlagBits::e1;
            depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
            depthAttachment.storeOp = vk::AttachmentStoreOp::eStore;
            depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
            depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
            depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
            depthAttachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

            depthRef.attachment = 1;
            depthRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        }

        vk::SubpassDescription subpass{};
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorRef;
        subpass.pDepthStencilAttachment = (depth) ? &depthRef : nullptr;

		std::vector<vk::AttachmentDescription> attachments;
		attachments.push_back(colorAttachment);
        if (depth) {
            attachments.push_back(depthAttachment);
        }

        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        attachmentInfos_.push_back(
            AttachmentInfo{
                colorAttachment, vk::ImageLayout::eColorAttachmentOptimal
            }
        );
        attachmentInfos_.push_back(
            AttachmentInfo{
                depthAttachment, vk::ImageLayout::eDepthStencilAttachmentOptimal
            }
        );

        renderPass_ = pDevice_->GetDevice().createRenderPassUnique(renderPassInfo);
        pDevice_->SetObjectName(
            (uint64_t)(VkRenderPass)(renderPass_.get()),
            vk::ObjectType::eRenderPass,
            name + "_RenderPass"
		);
	}

    RenderPass::RenderPass(
        const Device& device,
        std::string name,
        std::vector<SubPassInfo> subPassInfos,
        std::map<string, AttachmentInfo> attachmentNameToInfo
	) : pDevice_(&device)
    {
        vector<vk::AttachmentDescription> attachmentsDescs;
		std::map<string, int> attachmentNameToDescID;
        for (int i = 0; i < subPassInfos.size(); i++) {
            for (int j = 0; j < subPassInfos[i].attachmentInfos.size(); j++) {
				string attachmentName = subPassInfos[i].attachmentInfos[j];
                if (std::find(uniqueAttachmentNames_.begin(), uniqueAttachmentNames_.end(), attachmentName) == uniqueAttachmentNames_.end()) {
					int uniqueID = static_cast<int>(uniqueAttachmentNames_.size());
					uniqueAttachmentNames_.push_back(attachmentName);
                    attachmentsDescs.push_back(attachmentNameToInfo[attachmentName].attachmentDesc);
                    attachmentNameToDescID[attachmentName] = uniqueID;
                    if (attachmentNameToInfo[attachmentName].imageLayout == vk::ImageLayout::eColorAttachmentOptimal) {
                        numColorAttachments_++;
                    }
                    else if (attachmentNameToInfo[attachmentName].imageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
                        numDepthAttachments_++;
                    }
                    attachmentInfos_.push_back(attachmentNameToInfo[attachmentName]);
                }
            }
        }

        vector<vk::SubpassDescription> subPassDescs;
        subPassDescs.resize(subPassInfos.size());
        vector<vector<vk::AttachmentReference>> allColorRefs(subPassInfos.size());
        vector<optional<vk::AttachmentReference>> allDepthRefs(subPassInfos.size());
        for (int i = 0; i < subPassInfos.size(); i++) {
            for (int j = 0; j < subPassInfos[i].attachmentInfos.size(); j++) {
                string attachmentName = subPassInfos[i].attachmentInfos[j];
                if (attachmentNameToInfo[attachmentName].imageLayout == vk::ImageLayout::eColorAttachmentOptimal) {
                    allColorRefs[i].push_back(
                        vk::AttachmentReference{}
                        .setAttachment(attachmentNameToDescID[attachmentName])
                        .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
					);
                }
                else if (attachmentNameToInfo[attachmentName].imageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
                    allDepthRefs[i] = vk::AttachmentReference{}
                        .setAttachment(attachmentNameToDescID[attachmentName])
                        .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
                }
			}

			subPassDescs[i] = vk::SubpassDescription{}
				.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
				.setColorAttachmentCount(static_cast<uint32_t>(allColorRefs[i].size()))
				.setPColorAttachments(allColorRefs[i].data())
				.setPDepthStencilAttachment(allDepthRefs[i] ? &(*allDepthRefs[i]) : nullptr);
        }

		// NOTE : multi subpass synchronization is not supported yet.
        vk::SubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dependency.srcAccessMask = {};
        dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
		dependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;  // Synchronize within the pixel region

        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo
            .setAttachmentCount(attachmentsDescs.size())
            .setPAttachments(attachmentsDescs.data())
            .setSubpassCount(subPassDescs.size())
            .setPSubpasses(subPassDescs.data())
            .setDependencyCount(1)
            .setPDependencies(&dependency);

		renderPass_ = pDevice_->GetDevice().createRenderPassUnique(renderPassInfo);
        pDevice_->SetObjectName(
            (uint64_t)(VkRenderPass)(renderPass_.get()),
            vk::ObjectType::eRenderPass,
            name + "_RenderPass"
        );
    }

    vk::RenderPass RenderPass::GetRenderPass() const
    {
        return renderPass_.get();
    }

    int RenderPass::GetNumColorAttachments() const
    {
		return numColorAttachments_;
    }

    int RenderPass::GetNumAttachments() const
    {
        return numColorAttachments_ + numDepthAttachments_;
	}

    vector<AttachmentInfo> RenderPass::GetAttachmentInfos() const
    {
		return attachmentInfos_;
    }
}