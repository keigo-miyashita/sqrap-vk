#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Device;
	class RenderPass;
	class Swapchain;

	class GUI
	{
	private:
		const Device* pDevice_ = nullptr;
		vk::UniqueDescriptorPool imguiDescPool_;

		bool InitializeGUI(GLFWwindow* window, uint32_t minImageCount, uint32_t imageCount, RenderPassHandle pRenderPass);


	public:
		GUI(const Device& device, GLFWwindow* window, SwapchainHandle pSwapchain, RenderPassHandle pRenderPass);
		~GUI();
		void NewFrame();
		vk::DescriptorPool GetImguiDescPool() const;
	};
}