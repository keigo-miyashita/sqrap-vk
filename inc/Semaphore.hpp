#pragma once

#include "pch.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Device;

	class Semaphore
	{
	private:
		const Device* pDevice_ = nullptr;

		vk::UniqueSemaphore semaphore_;

	public:
		Semaphore(const Device& device);
		~Semaphore() = default;

		vk::Semaphore GetSemaphore() const;
	};
}