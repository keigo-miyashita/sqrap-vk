#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Device;

	class Fence
	{
	private:
		const Device* pDevice_ = nullptr;

		vk::UniqueFence fence_;

	public:
		Fence(const Device& device, bool signal = true);
		~Fence() = default;
		void Finished();
		void Reset();
		void Wait();

		vk::Fence GetFence() const;
	};
}