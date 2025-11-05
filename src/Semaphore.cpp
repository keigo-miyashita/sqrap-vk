#include "Semaphore.hpp"

#include "Device.hpp"

using namespace std;

namespace sqrp
{
	Semaphore::Semaphore(const Device& device, std::string name)
		: pDevice_(&device), name_(name)
	{
		semaphore_ = pDevice_->GetDevice().createSemaphoreUnique(
			vk::SemaphoreCreateInfo()
		);
		pDevice_->SetObjectName((uint64_t)(VkSemaphore)semaphore_.get(), vk::ObjectType::eSemaphore, name_);
	}

	vk::Semaphore Semaphore::GetSemaphore() const {
		return *semaphore_;
	}

}