#include "Fence.hpp"

#include "Device.hpp"

using namespace std;

namespace sqrp
{
	Fence::Fence(const Device& device, bool signal)
		: pDevice_(&device)
	{
		if (signal) {
			fence_ = pDevice_->GetDevice().createFenceUnique(
				vk::FenceCreateInfo()
				.setFlags(vk::FenceCreateFlagBits::eSignaled)
			);
		}
		else {
			fence_ = pDevice_->GetDevice().createFenceUnique(
				vk::FenceCreateInfo()
			);
		}
	}

	void Fence::Finished()
	{
		pDevice_->GetDevice().getFenceStatus(fence_.get());
	}

	void Fence::Reset()
	{
		pDevice_->GetDevice().resetFences(fence_.get());
	}

	void Fence::Wait()
	{
		if (pDevice_->GetDevice().waitForFences(fence_.get(), VK_TRUE, UINT64_MAX) != vk::Result::eSuccess) {
			throw std::runtime_error("Failed to wait for fence");
		}
	}

	vk::Fence Fence::GetFence() const
	{
		return fence_.get();
	}
}