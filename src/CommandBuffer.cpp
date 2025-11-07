#include "CommandBuffer.hpp"

#include "Buffer.hpp"
#include "DescriptorSet.hpp"
#include "FrameBuffer.hpp"
#include "Gui.hpp"
#include "Image.hpp"
#include "Mesh.hpp"
#include "Pipeline.hpp"
#include "RenderPass.hpp"
#include "Swapchain.hpp"

using namespace std;

namespace sqrp
{
	CommandBuffer::CommandBuffer(const Device& device, QueueContextType queueType)
		: pDevice_(&device), queueType_(queueType)
	{
		auto it = pDevice_->GetQueueContexts().find(queueType_);
		if (it == pDevice_->GetQueueContexts().end()) {
			throw std::runtime_error("No such queue context type");
		}
		const auto& context = it->second;
		commandBuffer_ = std::move(pDevice_->GetDevice().allocateCommandBuffersUnique(
			vk::CommandBufferAllocateInfo()
			.setCommandPool(context.commandPool.get())
			.setCommandBufferCount(1)
			.setLevel(vk::CommandBufferLevel::ePrimary)
		).front());
	}

	void CommandBuffer::Begin()
	{
		commandBuffer_->begin(
			vk::CommandBufferBeginInfo{}
		);
	}

	void CommandBuffer::Begin(vk::CommandBufferUsageFlags flag)
	{
		commandBuffer_->begin(
			vk::CommandBufferBeginInfo()
			.setFlags(flag)
		);
	}

	void CommandBuffer::End()
	{
		commandBuffer_->end();
	}

	void BeginRender(SwapchainHandle pSwapchain)
	{

	}

	void EndRender(SwapchainHandle pSwapchain)
	{

	}

	void CommandBuffer::BeginRenderPass(RenderPassHandle pRenderPass, FrameBufferHandle pFrameBuffer, uint32_t inflightIndex)
	{
		vk::RenderPassBeginInfo renderPassInfo{};
		renderPassInfo.renderPass = pRenderPass->GetRenderPass();
		renderPassInfo.framebuffer = pFrameBuffer->GetFrameBuffer(inflightIndex);
		renderPassInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
		renderPassInfo.renderArea.extent = vk::Extent2D{ pFrameBuffer->GetWidth() , pFrameBuffer->GetHeight()};
		std::vector<vk::ClearValue> clearValues(pRenderPass->GetNumAttachments());
		auto attachmentInfos = pRenderPass->GetAttachmentInfos();
		for (int i = 0; i < pRenderPass->GetNumAttachments(); i++) {
			auto attachmentInfo = attachmentInfos[i];
			clearValues[i] = vk::ClearValue();
			if (attachmentInfo.imageLayout == vk::ImageLayout::eColorAttachmentOptimal) {
				clearValues[i].color = vk::ClearColorValue(std::array<float, 4>{ 0.2f, 0.2f, 0.2f, 1.0f });
			}
			else if (attachmentInfo.imageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
				clearValues[i].depthStencil = vk::ClearDepthStencilValue{ 1.0f, 0 };
			}
		}
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		commandBuffer_->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
	}

	void CommandBuffer::EndRenderPass() {
		commandBuffer_->endRenderPass();
	}

	void CommandBuffer::BindPipeline(PipelineHandle pPipeline, vk::PipelineBindPoint pipelineBindPoint)
	{
		commandBuffer_->bindPipeline(pipelineBindPoint, pPipeline->GetPipeline());
	}

	void CommandBuffer::BindMeshBuffer(MeshBaseHandle pMesh)
	{
		commandBuffer_->bindVertexBuffers(0, pMesh->GetVertexBuffer()->GetBuffer(), { 0 });
		commandBuffer_->bindIndexBuffer(pMesh->GetIndexBuffer()->GetBuffer(), 0, vk::IndexType::eUint32);
	}

	void CommandBuffer::BindMeshBuffer(MeshBaseHandle pMesh, int vertexByteOffset, int indexByteOffset)
	{
		commandBuffer_->bindVertexBuffers(0, pMesh->GetVertexBuffer()->GetBuffer(), vertexByteOffset);
		commandBuffer_->bindIndexBuffer(pMesh->GetIndexBuffer()->GetBuffer(), indexByteOffset, vk::IndexType::eUint32);
	}

	void CommandBuffer::BindDescriptorSet(PipelineHandle pPipeline, DescriptorSetHandle pDescriptorSet, vk::PipelineBindPoint pipelineBindPoint)
	{
		commandBuffer_->bindDescriptorSets(
			pipelineBindPoint,
			pPipeline->GetPipelineLayout(),
			0,
			pDescriptorSet->GetDescriptorSet(),
			{}
		);
	}

	void CommandBuffer::PushConstants(PipelineHandle pPipeline, vk::ShaderStageFlags stageFlags, uint32_t size, const void* pValues)
	{
		commandBuffer_->pushConstants(
			pPipeline->GetPipelineLayout(),
			stageFlags,
			0,
			size,
			pValues
		);
	}

	void CommandBuffer::CopyBuffer(BufferHandle srcBuffer, BufferHandle dstBuffer)
	{
		commandBuffer_->copyBuffer(
			srcBuffer->GetBuffer(), dstBuffer->GetBuffer(),
			vk::BufferCopy()
			.setSize(min(srcBuffer->GetSize(), dstBuffer->GetSize()))
		);
	}

	void CommandBuffer::CopyBufferRegion(BufferHandle srcBuffer, vk::DeviceSize srcOffset, BufferHandle dstBuffer, vk::DeviceSize dstOffset, vk::DeviceSize size)
	{
		commandBuffer_->copyBuffer(
			srcBuffer->GetBuffer(), dstBuffer->GetBuffer(),
			vk::BufferCopy()
				.setSrcOffset(srcOffset)
				.setDstOffset(dstOffset)
				.setSize(size)
		);
	}

	void CommandBuffer::CopyBufferToImage(BufferHandle srcBuffer, ImageHandle dstImage)
	{
		vk::BufferImageCopy region{};
		region.setBufferOffset(0);
		region.setBufferRowLength(0);
		region.setBufferImageHeight(0);

		region.setImageSubresource(
			vk::ImageSubresourceLayers()
			.setAspectMask(dstImage->GetAspectFlags())
			.setMipLevel(0)
			.setBaseArrayLayer(0)
			.setLayerCount(1) // TODO: arrayLayers
		);

		region.setImageOffset(vk::Offset3D{ 0, 0, 0 });
		region.setImageExtent(dstImage->GetExtent3D());

		commandBuffer_->copyBufferToImage(
			srcBuffer->GetBuffer(),
			dstImage->GetImage(),
			vk::ImageLayout::eTransferDstOptimal,
			{ region }
		);
	}

	void CommandBuffer::SetScissor(uint32_t width, uint32_t height)
	{
		commandBuffer_->setScissor(0, vk::Rect2D{ {0, 0}, {width, height} });
	}

	void CommandBuffer::SetViewport(uint32_t width, uint32_t height)
	{
		commandBuffer_->setViewport(0, vk::Viewport{ 0.0f, static_cast<float>(height), static_cast<float>(width), -static_cast<float>(height), 0.0f, 1.0f });
	}

	void CommandBuffer::TransitionLayout(ImageHandle pImage, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
	{
		if (oldLayout == newLayout) return;
		vk::ImageMemoryBarrier barrier{};
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.image = pImage->GetImage();
		barrier.subresourceRange = {
			pImage->GetAspectFlags(),
			0, pImage->GetMipLevels(),  // mipLevels
			0, pImage->GetArrayLayers()   // arrayLayers
		};
		commandBuffer_->pipelineBarrier(
			vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands,
			{},
			{},
			{},
			{ barrier }
		);
		pImage->SetImageLayout(newLayout);
	}

	void CommandBuffer::TransitionLayout(vk::Image image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
	{
		vk::ImageMemoryBarrier barrier{};
		barrier.setOldLayout(oldLayout);
		barrier.setNewLayout(newLayout);
		barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
		barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
		barrier.setImage(image);
		barrier.setSubresourceRange(
			vk::ImageSubresourceRange()
			.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseMipLevel(0)
			.setLevelCount(1)
			.setBaseArrayLayer(0)
			.setLayerCount(1)
		);
		barrier.setSrcAccessMask(vk::AccessFlagBits::eNone);
		barrier.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

		commandBuffer_->pipelineBarrier(
			vk::PipelineStageFlagBits::eBottomOfPipe,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			{},
			{},
			{},
			{ barrier }
		);
	}

	void CommandBuffer::ImageBarrier(
		ImageHandle pImage,
		vk::ImageLayout oldLayout,
		vk::ImageLayout newLayout,
		vk::PipelineStageFlags srcStageMask,
		vk::PipelineStageFlags dstStageMask,
		vk::AccessFlags srcAccessMask,
		vk::AccessFlags dstAccessMask
	)
	{
		vk::ImageMemoryBarrier barrier{};
		barrier.setOldLayout(oldLayout);
		barrier.setNewLayout(newLayout);
		barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
		barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
		barrier.setImage(pImage->GetImage());
		barrier.setSubresourceRange(
			vk::ImageSubresourceRange()
			.setAspectMask(pImage->GetAspectFlags())
			.setBaseMipLevel(0)
			.setLevelCount(pImage->GetMipLevels())
			.setBaseArrayLayer(0)
			.setLayerCount(pImage->GetArrayLayers())
		);
		barrier.setSrcAccessMask(srcAccessMask);
		barrier.setDstAccessMask(dstAccessMask);
		commandBuffer_->pipelineBarrier(
			srcStageMask, dstStageMask,
			{},
			{},
			{},
			{ barrier }
		);
	}

	void CommandBuffer::DrawMesh(MeshBaseHandle pMesh, int numIndices)
	{
		commandBuffer_->drawIndexed(static_cast<uint32_t>(numIndices), 1, 0, 0, 0);
	}

	void CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount)
	{
		commandBuffer_->draw(vertexCount, instanceCount, 0, 0);
	}

	void CommandBuffer::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
	{
		commandBuffer_->dispatch(groupCountX, groupCountY, groupCountZ);
	}

	void CommandBuffer::DrawGui(GUI& gui)
	{
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), this->GetCommandBuffer());
	}

	vk::CommandBuffer CommandBuffer::GetCommandBuffer() const
	{
		return commandBuffer_.get();
	}
}