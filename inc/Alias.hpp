#pragma once

namespace sqrp
{
	class Buffer;
	class CommandBuffer;
	class Device;
	class Fence;
	class Image;
	class RenderPass;
	class Semaphore;
	class Shader;
	class Swapchain;

	using BufferHandle = std::shared_ptr<Buffer>;
	using CommandBufferHandle = std::shared_ptr<CommandBuffer>;
	using FenceHandle = std::shared_ptr<Fence>;
	using ImageHandle = std::shared_ptr<Image>;
	using RenderPassHandle = std::shared_ptr<RenderPass>;
	using SemaphoreHandle = std::shared_ptr<Semaphore>;
	using ShaderHandle = std::shared_ptr<Shader>;
	using SwapchainHandle = std::shared_ptr<Swapchain>;
}