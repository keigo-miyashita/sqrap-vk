#pragma once

namespace sqrp
{
	class CommandBuffer;
	class Device;
	class Fence;
	class Shader;
	class Swapchain;

	using CommandBufferHandle = std::shared_ptr<CommandBuffer>;
	using FenceHandle = std::shared_ptr<Fence>;
	using ShaderHandle = std::shared_ptr<Shader>;
	using SwapchainHandle = std::shared_ptr<Swapchain>;
}