#include "Gui.hpp"

#include "CommandBuffer.hpp"
#include "Device.hpp"
#include "RenderPass.hpp"
#include "Swapchain.hpp"

using namespace std;

namespace sqrp
{
	bool GUI::InitializeGUI(GLFWwindow* window, uint32_t minImageCount, uint32_t imageCount, RenderPassHandle pRenderPass)
	{
		vk::DescriptorPoolSize poolSize[] = 
		{
			{ vk::DescriptorType::eSampler, 1000 },
			{ vk::DescriptorType::eCombinedImageSampler, 1000 },
			{ vk::DescriptorType::eSampledImage, 1000 },
			{ vk::DescriptorType::eStorageImage, 1000 },
			{ vk::DescriptorType::eUniformTexelBuffer, 1000 },
			{ vk::DescriptorType::eStorageTexelBuffer, 1000 },
			{ vk::DescriptorType::eUniformBuffer, 1000 },
			{ vk::DescriptorType::eStorageBuffer, 1000 },
			{ vk::DescriptorType::eUniformBufferDynamic, 1000 },
			{ vk::DescriptorType::eStorageBufferDynamic, 1000 },
			{ vk::DescriptorType::eInputAttachment, 1000 }
		};
		vk::DescriptorPoolCreateInfo descPoolInfo = {};
		descPoolInfo
			.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
			.setMaxSets(1000 * IM_ARRAYSIZE(poolSize))
			.setPoolSizeCount(static_cast<uint32_t>(IM_ARRAYSIZE(poolSize)))
			.setPPoolSizes(poolSize);

		imguiDescPool_ = pDevice_->GetDevice().createDescriptorPoolUnique(descPoolInfo);

		if (ImGui::CreateContext() == nullptr) {
			throw std::runtime_error("Failed to ImGui::CreateContext");
			return false;
		}

		bool binResult = ImGui_ImplGlfw_InitForVulkan(window, true);
		if (!binResult) {
			throw std::runtime_error("Failed to ImGui_ImplWin32_Init");
			return false;
		}

		/*auto imguiCPUHandle = imguiDescHeap_->GetCPUDescriptorHandleForHeapStart();
		auto imguiGPUHandle = imguiDescHeap_->GetGPUDescriptorHandleForHeapStart();
		binResult = ImGui_ImplDX12_Init(pDevice_->GetDevice().Get(), 2, DXGI_FORMAT_R8G8B8A8_UNORM, imguiDescHeap_.Get(), imguiCPUHandle, imguiGPUHandle);
		if (!binResult) {
			throw std::runtime_error("Failed to ImGui_ImplDX12_Init");
			return false;
		}*/
		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = pDevice_->GetInstance();
		initInfo.PhysicalDevice = pDevice_->GetPhysicalDevice();
		initInfo.Device = pDevice_->GetDevice();
		const auto& queueContext = pDevice_->GetQueueContexts();
		initInfo.QueueFamily = queueContext.at(QueueContextType::General).queueFamilyIndex;
		initInfo.Queue = queueContext.at(QueueContextType::General).queue;
		initInfo.PipelineCache = nullptr;
		initInfo.DescriptorPool = imguiDescPool_.get();
		initInfo.DescriptorPoolSize = 0;
		initInfo.MinImageCount = minImageCount;
		initInfo.ImageCount = imageCount;
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		initInfo.Allocator = nullptr;
		initInfo.RenderPass = pRenderPass->GetRenderPass();

		ImGui_ImplVulkan_Init(&initInfo);

		CommandBufferHandle commandBuffer = pDevice_->CreateCommandBuffer(QueueContextType::General);
		ImGui_ImplVulkan_CreateFontsTexture();

		return true;
	}

	GUI::GUI(const Device& device, GLFWwindow* window, SwapchainHandle pSwapchain, RenderPassHandle pRenderPass)
		: pDevice_(&device)
	{
		InitializeGUI(window, pSwapchain->GetMinImageCount(), pSwapchain->GetInflightCount(), pRenderPass);
	}

	GUI::~GUI()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void GUI::NewFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	vk::DescriptorPool GUI::GetImguiDescPool() const
	{
		return imguiDescPool_.get();
	}
}