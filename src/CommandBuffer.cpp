#include "CommandBuffer.hpp"

#include "Buffer.hpp"
#include "DescriptorSet.hpp"
#include "FrameBuffer.hpp"
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
		//commandBuffer = CreateCommandBuffer(pDevice_->GetDevice(), context.commandPool.get());
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

	void CommandBuffer::BeginRenderPass(RenderPassHandle pRenderPass, FrameBufferHandle pFrameBuffer)
	{
		vk::ClearValue clearColor(std::array<float, 4>{0.2f, 0.2f, 0.2f, 1.0f});
		vk::ClearValue clearDepth({ 1.0f, 0 });

		vk::RenderPassBeginInfo renderPassInfo{};
		renderPassInfo.renderPass = pRenderPass->GetRenderPass();
		renderPassInfo.framebuffer = pFrameBuffer->GetFrameBuffer(pFrameBuffer->GetSwapchain()->GetImageIndex());
		renderPassInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
		renderPassInfo.renderArea.extent = pFrameBuffer->GetSwapchain()->GetExtent2D();
		std::array<vk::ClearValue, 2> clearValues = { clearColor, clearDepth };
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

	void CommandBuffer::BindMeshBuffer(MeshHandle pMesh)
	{
		commandBuffer_->bindVertexBuffers(0, pMesh->GetVertexBuffer()->GetBuffer(), { 0 });
		commandBuffer_->bindIndexBuffer(pMesh->GetIndexBuffer()->GetBuffer(), 0, vk::IndexType::eUint32);
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

	void CommandBuffer::SetScissor(uint32_t width, uint32_t height)
	{
		commandBuffer_->setScissor(0, vk::Rect2D{ {0, 0}, {width, height} });
	}

	void CommandBuffer::SetViewport(uint32_t width, uint32_t height)
	{
		commandBuffer_->setViewport(0, vk::Viewport{ 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f });
	}

	void CommandBuffer::TransitionLayout(ImageHandle pImage, vk::ImageLayout newLayout)
	{
		if (pImage->GetImageLayout() == newLayout) return;
		pImage->SetImageLayout(newLayout);
		vk::ImageMemoryBarrier barrier{};
		barrier.oldLayout = pImage->GetImageLayout();
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
		barrier.image = pImage->GetImage();
		barrier.subresourceRange = {
			pImage->GetAspectFlags(),
			0, 1,  // mipLevels
			0, 1   // arrayLayers
		};
		commandBuffer_->pipelineBarrier(
			vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands,
			{},
			{},
			{},
			{ barrier }
		);
	}

	void CommandBuffer::DrawMesh(MeshHandle pMesh)
	{
		commandBuffer_->drawIndexed(static_cast<uint32_t>(pMesh->GetNumIndices()), 1, 0, 0, 0);
	}

	vk::CommandBuffer CommandBuffer::GetCommandBuffer() const
	{
		return commandBuffer_.get();
	}
}