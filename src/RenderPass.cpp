#include "RenderPass.hpp"

#include "Device.hpp"
#include "Swapchain.hpp"

using namespace std;

namespace sqrp
{
	RenderPass::RenderPass(
        const Device& device,
        SwapchainHandle pSwapchain
	)
		: pDevice_(&device)
	{
        numColorAttachments_ = 1;
		numDepthAttachments_ = 1;

        // カラーアタッチメント
        vk::AttachmentDescription colorAttachment{};
        colorAttachment.format = pSwapchain->GetSurfaceFormat();
        colorAttachment.samples = vk::SampleCountFlagBits::e1;
        colorAttachment.loadOp = vk::AttachmentLoadOp::eClear; // 初期化時にクリア
        colorAttachment.storeOp = vk::AttachmentStoreOp::eStore; // 書き戻す
        colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
        colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        // 深度アタッチメント
        vk::AttachmentDescription depthAttachment{};
        depthAttachment.format = vk::Format::eD32Sfloat;
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
	}

    RenderPass::RenderPass(
        const Device& device,
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
					//cout << "colorRef: " << globalIndex << endl;
                }
                else if (attachmentNameToInfo[attachmentName].imageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
                    allDepthRefs[i] = vk::AttachmentReference{}
                        .setAttachment(attachmentNameToDescID[attachmentName])
                        .setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
					//cout << "depthRef: " << globalIndex << endl;
                }
			}

			subPassDescs[i] = vk::SubpassDescription{}
				.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
				.setColorAttachmentCount(static_cast<uint32_t>(allColorRefs[i].size()))
				.setPColorAttachments(allColorRefs[i].data())
				.setPDepthStencilAttachment(allDepthRefs[i] ? &(*allDepthRefs[i]) : nullptr);
        }

        vk::RenderPassCreateInfo renderPassInfo{};
        renderPassInfo
            .setAttachmentCount(attachmentsDescs.size())
            .setPAttachments(attachmentsDescs.data())
            .setSubpassCount(subPassDescs.size())
            .setPSubpasses(subPassDescs.data());

		renderPass_ = pDevice_->GetDevice().createRenderPassUnique(renderPassInfo);
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