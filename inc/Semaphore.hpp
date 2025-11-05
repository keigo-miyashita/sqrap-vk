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

		std::string name_ = "Semaphore";
		vk::UniqueSemaphore semaphore_;

	public:
		Semaphore(const Device& device, std::string name = "Semaphore");
		~Semaphore() = default;

		vk::Semaphore GetSemaphore() const;
	};
}