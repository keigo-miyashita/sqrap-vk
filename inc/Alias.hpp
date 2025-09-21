#pragma once

namespace sqrp
{
	class CommandBuffer;
	class Device;
	class Fence;
	class Semaphore;
	class Shader;
	class Swapchain;

	using CommandBufferHandle = std::shared_ptr<CommandBuffer>;
	using FenceHandle = std::shared_ptr<Fence>;
	using SemaphoreHandle = std::shared_ptr<Semaphore>;
	using ShaderHandle = std::shared_ptr<Shader>;
	using SwapchainHandle = std::shared_ptr<Swapchain>;
}