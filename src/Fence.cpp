#include "Fence.hpp"

#include "Device.hpp"

using namespace std;

namespace sqrp
{
	Fence::Fence(const Device& device, bool signal = true)
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

	FenceHandle Fence::CreateFence(const Device& device, bool signal = true)
	{
		return std::make_shared<Fence>(device, signal);
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
		pDevice_->GetDevice().waitForFences(fence_.get(), VK_TRUE, std::numeric_limits<uint64_t>::max());
	}

	vk::Fence GetFence() const
	{
		return fence_.get();
	}
}