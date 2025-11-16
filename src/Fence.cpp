#include "Fence.hpp"

#include "Device.hpp"

using namespace std;

namespace sqrp
{
	Fence::Fence(const Device& device, std::string name, bool signal)
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
		pDevice_->SetObjectName(
			(uint64_t)(VkFence)(fence_.get()),
			vk::ObjectType::eFence,
			name + "_Fence"
		);
	}

	void Fence::Finished()
	{
		auto result = pDevice_->GetDevice().getFenceStatus(fence_.get());

		// If the fence is neither signaled nor unsignaled, something wrong
		if (result != vk::Result::eSuccess && result != vk::Result::eNotReady) {
			throw std::runtime_error("Failed to get fence status");
		}
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