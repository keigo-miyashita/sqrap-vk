#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Device;
	class Swapchain;

	class RenderPass
	{
	private:
		const Device* pDevice_ = nullptr;

		vk::UniqueRenderPass renderPass_;


	public:
		RenderPass(
			const Device& device,
			SwapchainHandle pSwapchain
		);
		~RenderPass() = default;

		vk::RenderPass GetRenderPass() const;
	};
}