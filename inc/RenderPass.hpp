#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Device;

	class RenderPass
	{
	private:
		const Device* pDevice_ = nullptr;

		vk::UniqueRenderPass renderPass_;


	public:
		RenderPass(
			const Device& device
		);
		~RenderPass() = default;
	};
}