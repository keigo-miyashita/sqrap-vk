#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Device;
	class Swapchain;

	struct AttachmentInfo
	{
		vk::AttachmentDescription attachmentDesc;
		vk::ImageLayout imageLayout;
	};

	struct SubPassInfo
	{
		std::vector<std::string> attachmentInfos;
	};

	class RenderPass
	{
	private:
		const Device* pDevice_ = nullptr;

		vk::UniqueRenderPass renderPass_;
		std::vector<std::string> uniqueAttachmentNames_;
		std::vector<AttachmentInfo> attachmentInfos_;
		int numColorAttachments_ = 0;
		int numDepthAttachments_ = 0;


	public:
		RenderPass(
			const Device& device,
			std::string name,
			SwapchainHandle pSwapchain,
			bool depth = true
		);
		RenderPass(
			const Device& device,
			std::string name,
			std::vector<SubPassInfo> subPassInfos,
			std::map<std::string, AttachmentInfo> attachmentNameToInfo
		);
		~RenderPass() = default;

		vk::RenderPass GetRenderPass() const;
		int GetNumColorAttachments() const;
		int GetNumAttachments() const;
		std::vector<AttachmentInfo> GetAttachmentInfos() const;
	};
}