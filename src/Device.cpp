#include "Device.hpp"

#include "CommandBuffer.hpp"
#include "Buffer.hpp"
#include "Fence.hpp"
#include "FrameBuffer.hpp"
#include "Image.hpp"
#include "Pipeline.hpp"
#include "RenderPass.hpp"
#include "Semaphore.hpp"
#include "Shader.hpp"
#include "Swapchain.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

using namespace std;

namespace
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pData)
	{
		std::cerr << "validation layer : " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}
}

namespace sqrp
{
	bool Device::isDeviceExtensionSupport(vk::PhysicalDevice physDev)
	{
		set<string> requiredExtensions{ requestDeviceExtensions_.begin(), requestDeviceExtensions_.end() };

		for (const auto& extension : physDev.enumerateDeviceExtensionProperties()) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	bool Device::isDeviceSuitable(vk::PhysicalDevice physDev)
	{
		if (!isDeviceExtensionSupport(physDev)) {
			return false;
		}
		if (physDev.getSurfaceFormatsKHR(surface_.get()).empty() || physDev.getSurfacePresentModesKHR(surface_.get()).empty()) {
			return false;
		}

		return true;
	}

	Device::Device()
	{
		
	}

	bool Device::Init(Application application)
	{
		// Setup dynamic library loader
		static vk::DynamicLoader dl;
		auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
		VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

		vk::ApplicationInfo appInfo = vk::ApplicationInfo()
			.setPApplicationName(application.GetAppName().c_str())
			.setApplicationVersion(VK_MAKE_API_VERSION(1, 0, 0, 0))
			.setApiVersion(VK_API_VERSION_1_3);

		// Get extension for GLFW
		uint32_t extensionCount = 0;
		auto ppExtensionNames = glfwGetRequiredInstanceExtensions(&extensionCount);
		// Using range constructor of vector
		for (int i = 0; i < extensionCount; i++) {
			requestInstanceExtensions_.push_back(ppExtensionNames[i]);
		}
		// Get extension for debug
		requestInstanceExtensions_.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		requestDeviceExtensions_.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		// Create instance
		// NOTE : Turn on validation layer at debug build
		layers_.push_back("VK_LAYER_KHRONOS_validation");
		instance_ = vk::createInstanceUnique(vk::InstanceCreateInfo()
			.setPApplicationInfo(&appInfo)
			.setEnabledLayerCount(layers_.size())
			.setPpEnabledLayerNames(layers_.data())
			.setEnabledExtensionCount(requestInstanceExtensions_.size())
			.setPpEnabledExtensionNames(requestInstanceExtensions_.data())
		);
		VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance_);

		// Create Debug Messager
		vk::DebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.setMessageType(
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
		createInfo.setMessageSeverity(
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
		createInfo.setPfnUserCallback(&debugCallback);
		debugMessenger_ = instance_->createDebugUtilsMessengerEXTUnique(createInfo);

		// Create surface
		VkSurfaceKHR rawSurface;
		if (glfwCreateWindowSurface(instance_.get(), application.GetPWindow(), nullptr, &rawSurface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
		surface_ = vk::UniqueSurfaceKHR(rawSurface, instance_.get());

		// Select physical device
		for (const auto& physDev : instance_->enumeratePhysicalDevices()) {
			if (isDeviceSuitable(physDev)) { // == VK_SUCCESS ???
				physicalDevice_ = physDev;
			}
		}
		if (!physicalDevice_) {
			throw std::runtime_error("Failed to select device!");
		}

		// Find queue family
		auto queueFamilies = physicalDevice_.getQueueFamilyProperties();
		// Find Queue family index has graphics queue and present queue
		for (uint32_t i = 0; i < queueFamilies.size(); i++) {
			auto isSupportGraphics = queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics;
			auto isSupportCompute = queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute;
			auto isSupportTransfer = queueFamilies[i].queueFlags & vk::QueueFlagBits::eTransfer;
			if (surface_) {
				auto isSupportPresent = physicalDevice_.getSurfaceSupportKHR(i, surface_.get());
				if (isSupportGraphics && isSupportCompute && isSupportTransfer && isSupportPresent) {
					queueContexts_[QueueContextType::General] = { i, {}, 0, {} };
					if (queueFamilies[i].queueCount > 1) {
						queueContexts_[QueueContextType::Compute] = { i, {}, 1, {} };
					}
					continue;
				}
				if (isSupportGraphics && isSupportPresent) {
					queueContexts_[QueueContextType::Graphics] = { i, {}, 0, {} };
					continue;
				}
			}
		}

		// Create logical device
		vector<vk::DeviceQueueCreateInfo> queueCreateInfos = {};
		struct QueueFamilyInfo {
			uint32_t familyIndex;
			uint32_t count;
		};
		map<uint32_t, uint32_t> queueFamilyInfos; /*pair<>*/
		for (const auto& [type, context] : queueContexts_) {
			if (queueFamilyInfos.find(context.queueFamilyIndex) != queueFamilyInfos.end()) {
				queueFamilyInfos[context.queueFamilyIndex]++;
			}
			else {
				queueFamilyInfos[context.queueFamilyIndex] = 1;
			}
		}
		cout << "queueFamilyInfos.size() = " << queueFamilyInfos.size() << endl;
		float queuePriority = 1.0f;
		vector<float> queuePriorities;
		for (const auto [familyIndex, count] : queueFamilyInfos) {
			queuePriorities.resize(count);
			for (uint32_t i = 0; i < count; i++) {
				queuePriorities[i] = queuePriority;
			}
			queueCreateInfos.push_back(
				vk::DeviceQueueCreateInfo{}
				.setQueueFamilyIndex(familyIndex)
				.setQueueCount(count)
				.setPQueuePriorities(queuePriorities.data())
			);
		}
		vk::DeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo
			.setPQueueCreateInfos(queueCreateInfos.data())
			.setQueueCreateInfoCount(queueCreateInfos.size()) // If you need multi queue when async compute, set multivalue
			.setPEnabledExtensionNames(requestDeviceExtensions_);
		// NOTE : FIX!
		vk::StructureChain<vk::DeviceCreateInfo/*, features*/> createInfoChain{
			deviceCreateInfo,
			//vk::PhysicalDevice
		};

		device_ = physicalDevice_.createDeviceUnique(createInfoChain.get<vk::DeviceCreateInfo>());
		VULKAN_HPP_DEFAULT_DISPATCHER.init(device_.get());

		for (auto& [type, context] : queueContexts_) {
			context.queue = device_->getQueue(context.queueFamilyIndex, context.queueIndex);
			context.commandPool = device_->createCommandPoolUnique(
				vk::CommandPoolCreateInfo()
				.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
				.setQueueFamilyIndex(context.queueFamilyIndex)
			);
		}

		VmaAllocatorCreateInfo allocatorInfo{};
		allocatorInfo.physicalDevice = physicalDevice_;
		allocatorInfo.device = device_.get();
		allocatorInfo.instance = instance_.get();
		allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;

		if (vmaCreateAllocator(&allocatorInfo, &allocator_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create VMA allocator");
		}

		return true;
	}

	BufferHandle Device::CreateBuffer(int size,
		vk::BufferUsageFlags usage,
		VmaMemoryUsage memoryUsage,
		VmaAllocationCreateFlags allocationFlags) const
	{
		return std::make_shared<Buffer>(*this, size, usage, memoryUsage, allocationFlags);
	}

	CommandBufferHandle Device::CreateCommandBuffer(QueueContextType queueType) const
	{
		return std::make_shared<CommandBuffer>(*this, queueType);
	}

	DescriptorSetHandle Device::CreateDescriptorSet(std::vector<DescriptorSetCreateInfo> descriptorSetCreateInfos) const
	{
		return std::make_shared<DescriptorSet>(*this, descriptorSetCreateInfos);
	}

	FenceHandle Device::CreateFence(bool signal) const
	{
		return std::make_shared<Fence>(*this, signal);
	}

	FrameBufferHandle Device::CreateFrameBuffer(SwapchainHandle pSwapchain, RenderPassHandle pRenderPass, int numAttachmentBuffers) const
	{
		return std::make_shared<FrameBuffer>(*this, pSwapchain, pRenderPass, numAttachmentBuffers);
	}

	ImageHandle Device::CreateImage(
		vk::Extent3D extent3D,
		vk::ImageType imageType,
		vk::ImageUsageFlags usage,
		vk::Format format,
		vk::ImageLayout imageLayout,
		vk::ImageAspectFlags aspectFlags,
		int mipLevels,
		int arrayLayers,
		vk::SampleCountFlagBits samples,
		vk::ImageTiling tiling,
		vk::SamplerCreateInfo samplerCreateInfo
	) const
	{
		return std::make_shared<Image>(*this, extent3D, imageType, usage, format, imageLayout, aspectFlags, mipLevels, arrayLayers, samples, tiling, samplerCreateInfo);
	}

	MeshHandle Device::CreateMesh(std::string modelPath) const
	{
		return std::make_shared<Mesh>(*this, modelPath);
	}

	MeshHandle Device::CreateMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) const
	{
		return std::make_shared<Mesh>(*this, vertices, indices);
	}

	PipelineHandle Device::CreatePipeline(
		RenderPassHandle pRenderPass,
		SwapchainHandle pSwapchain,
		ShaderHandle pVertexShader,
		ShaderHandle pPixelShader,
		DescriptorSetHandle pDescriptorSet
	) const
	{
		return std::make_shared<Pipeline>(*this, pRenderPass, pSwapchain, pVertexShader, pPixelShader, pDescriptorSet);
	}

	RenderPassHandle Device::CreateRenderPass(SwapchainHandle pSwapchain) const
	{
		return std::make_shared<RenderPass>(*this, pSwapchain);
	}

	SemaphoreHandle Device::CreateSemaphore() const
	{
		return std::make_shared<Semaphore>(*this);
	}

	ShaderHandle Device::CreateShader(const Compiler& compiler, const std::string& fileName, ShaderType shaderType) const
	{
		return std::make_shared<Shader>(*this, compiler, fileName, shaderType);
	}

	SwapchainHandle Device::CreateSwapchain(uint32_t width, uint32_t height) const
	{
		return std::make_shared<Swapchain>(*this, width, height);
	}

	void Device::Submit(
		QueueContextType type,
		CommandBufferHandle pCommandBuffer,
		const std::vector<vk::PipelineStageFlags>& waitDstStageMasks,
		const std::vector<vk::Semaphore>& pWaitSemaphores,
		const std::vector<vk::Semaphore>& pSignalSemaphores,
		FenceHandle pFence
	) const
	{
		vk::Queue queue = queueContexts_.at(type).queue;
		auto commandBuffer = pCommandBuffer->GetCommandBuffer();

		vk::SubmitInfo submitInfo{};
		submitInfo
			.setCommandBuffers(commandBuffer)
			.setWaitSemaphoreCount(pWaitSemaphores.size())
			.setPWaitSemaphores(pWaitSemaphores.data())
			.setPWaitDstStageMask(waitDstStageMasks.data())
			.setSignalSemaphoreCount(pSignalSemaphores.size())
			.setPSignalSemaphores(pSignalSemaphores.data());

		if (pFence) {
			queue.submit(submitInfo, pFence->GetFence());
		}
		else {
			queue.submit(submitInfo, nullptr);
		}
	}

	void Device::Submit(
		QueueContextType type,
		CommandBufferHandle pCommandBuffer,
		vk::PipelineStageFlags waitDstStageMask,
		SemaphoreHandle pWaitSemaphores,
		SemaphoreHandle pSignalSemaphores,
		FenceHandle pFence
	) const
	{
		vk::Queue queue = queueContexts_.at(type).queue;
		auto commandBuffer = pCommandBuffer->GetCommandBuffer();

		vk::SubmitInfo submitInfo{};
		submitInfo.setCommandBuffers(commandBuffer);
		if (waitDstStageMask != vk::PipelineStageFlagBits::eNone) {
			submitInfo.setPWaitDstStageMask(&waitDstStageMask);
		}
		if (pWaitSemaphores) {
			auto waitSemaphores = pWaitSemaphores->GetSemaphore();
			submitInfo.setWaitSemaphores(waitSemaphores);
		}
		if (pSignalSemaphores) {
			auto signalSemaphore = pSignalSemaphores->GetSemaphore();
			submitInfo.setSignalSemaphores(signalSemaphore);
		}

		if (pFence) {
			queue.submit(submitInfo, pFence->GetFence());
		}
		else {
			queue.submit(submitInfo, nullptr);
		}
	}

	void Device::WaitIdle(QueueContextType type) const
	{
		queueContexts_.at(type).queue.waitIdle();
	}

	void Device::OneTimeSubmit(std::function<void(CommandBufferHandle pCommandBuffer)>&& command) const
	{
		QueueContextType selectedType = QueueContextType::General;
		for (const auto& [type, context] : queueContexts_) {
			if (type == QueueContextType::General || type == QueueContextType::Graphics) {
				selectedType = type;
				break;
			}
		}

		CommandBufferHandle commandBuffer = CreateCommandBuffer(selectedType);

		vk::CommandBufferBeginInfo beginInfo{};
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		commandBuffer->Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

		command(commandBuffer);

		commandBuffer->End();

		Submit(selectedType, commandBuffer);

		WaitIdle(selectedType);
	}

	void Device::SetObjectName(uint64_t object, vk::ObjectType objectType, const std::string& name) const
	{
		if (!debugMessenger_) return;
		vk::DebugUtilsObjectNameInfoEXT nameInfo{};
		nameInfo
			.setObjectType(objectType)
			.setObjectHandle(object)
			.setPObjectName(name.c_str());
		device_->setDebugUtilsObjectNameEXT(nameInfo);
	}

	VmaAllocator Device::GetAllocator() const
	{
		return allocator_;;
	}

	vk::PhysicalDevice Device::GetPhysicalDevice() const
	{
		return physicalDevice_;
	}

	vk::Device Device::GetDevice() const
	{
		return device_.get();
	}

	vk::SurfaceKHR Device::GetSurface() const
	{
		return surface_.get();
	}

	const std::map<QueueContextType, QueueContext>& Device::GetQueueContexts() const
	{
		return queueContexts_;
	}

	vk::Queue Device::GetQueue(QueueContextType type) const
	{
		return queueContexts_.at(type).queue;
	}

	/*uint32_t Device::GetGraphicsQueueFamilyIndex() const
	{
		return graphicsQueueFamilyIndex_;
	}

	uint32_t Device::GetPresentQueueFamilyIndex() const
	{
		return presentQueueFamilyIndex_;
	}*/
}