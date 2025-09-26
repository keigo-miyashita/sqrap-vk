#pragma once

namespace sqrp
{
	class Buffer;
	class CommandBuffer;
	class DescriptorSet;
	class Device;
	class Fence;
	class FrameBuffer;
	class Image;
	class Mesh;
	class PipeLine;
	class RenderPass;
	class Semaphore;
	class Shader;
	class Swapchain;

	using BufferHandle = std::shared_ptr<Buffer>;
	using CommandBufferHandle = std::shared_ptr<CommandBuffer>;
	using DescriptorSetHandle = std::shared_ptr<DescriptorSet>;
	using FenceHandle = std::shared_ptr<Fence>;
	using FrameBufferHandle = std::shared_ptr<FrameBuffer>;
	using ImageHandle = std::shared_ptr<Image>;
	using MeshHandle = std::shared_ptr<Mesh>;
	using PipeLineHandle = std::shared_ptr<PipeLine>;
	using RenderPassHandle = std::shared_ptr<RenderPass>;
	using SemaphoreHandle = std::shared_ptr<Semaphore>;
	using ShaderHandle = std::shared_ptr<Shader>;
	using SwapchainHandle = std::shared_ptr<Swapchain>;
}