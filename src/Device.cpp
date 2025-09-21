#include "Device.hpp"

#include "Semaphore.hpp"
#include "Swapchain.hpp"
#include "Fence.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

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
			
			/*if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
				graphicsQueueFamilyIndex_ = i;
			}*/
		}
		//if (graphicsQueueFamilyIndex_ == -1) {
		//	throw std::runtime_error("No graphics queue family!");
		//}

		//for (uint32_t i = 0; i < queueFamilies.size(); i++) {
		//	// Check selected physical device can handle the surface
		//	vk::Bool32 presentSupport = physicalDevice_.getSurfaceSupportKHR(i, surface_.get());
		//	if (presentSupport) {
		//		presentQueueFamilyIndex_ = i;
		//	}
		//}
		//if (presentQueueFamilyIndex_ == -1) {
		//	throw std::runtime_error("No present queue family!");
		//}

		//// NOTE : Fix to turn on compute queue only using async compute
		//queueFamilies = physicalDevice_.getQueueFamilyProperties();
		//// Find Queue family index has graphics queue and present queue
		//for (uint32_t i = 0; i < queueFamilies.size(); i++) {
		//	if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute) {
		//		computeQueueFamilyIndex_ = i;
		//	}
		//}
		//if (computeQueueFamilyIndex_ == -1) {
		//	throw std::runtime_error("No compute queue family!");
		//}

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
		/*for (auto& element : queueContexts_) {
			auto& type = element.first;
			auto& context = element.second;
			context.queue = device_->getQueue(context.queueFamilyIndex, context.queueIndex);
		}*/
		/*computeQueue_ = device_->getQueue(computeQueueFamilyIndex_, 0);
		computeCommandPool_ = device_->createCommandPoolUnique(
			vk::CommandPoolCreateInfo()
			.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
			.setQueueFamilyIndex(computeQueueFamilyIndex_)
		);
		graphicsQueue_ = device_->getQueue(graphicsQueueFamilyIndex_, 0);
		graphicsCommandPool_ = device_->createCommandPoolUnique(
			vk::CommandPoolCreateInfo()
			.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
			.setQueueFamilyIndex(graphicsQueueFamilyIndex_)
		);
		presentQueue_ = device_->getQueue(presentQueueFamilyIndex_, 0);
		presentCommandPool_ = device_->createCommandPoolUnique(
			vk::CommandPoolCreateInfo()
			.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
			.setQueueFamilyIndex(presentQueueFamilyIndex_)
		);*/

		return true;
	}

	FenceHandle Device::CreateFence(bool signal)
	{
		return std::make_shared<Fence>(*this, signal);
	}

	SemaphoreHandle Device::CreateSemaphore()
	{
		return std::make_shared<Semaphore>(*this);
	}

	SwapchainHandle Device::CreateSwapchain(uint32_t width, uint32_t height)
	{
		return std::make_shared<Swapchain>(*this, width, height);
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

	const std::unordered_map<QueueContextType, QueueContext>& Device::GetQueueContexts() const
	{
		return queueContexts_;
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