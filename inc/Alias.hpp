#pragma once

namespace sqrp
{
	class Command;
	class Device;
	class Shader;
	class Swapchain;

	using CommandHandle = std::shared_ptr<Command>;
	using ShaderHandle = std::shared_ptr<Shader>;
	using SwapchainHandle = std::shared_ptr<Swapchain>;
}