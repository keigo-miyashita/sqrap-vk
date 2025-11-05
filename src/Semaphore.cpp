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
	}

	vk::Semaphore Semaphore::GetSemaphore() const {
		return *semaphore_;
	}

}