#include "Semaphore.hpp"

#include "Device.hpp"

using namespace std;

namespace sqrp
{
	Semaphore::Semaphore(const Device& device)
		: pDevice_(&device)
	{
		semaphore_ = pDevice_->GetDevice().createSemaphoreUnique(
			vk::SemaphoreCreateInfo()
		);
	}

	vk::Semaphore Semaphore::GetSemaphore() const {
		return *semaphore_;
	}

}