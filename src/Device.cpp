#include "Device.hpp"

#define VULKAN_HPP_DISPATCH_DYNAMIC 1

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

	Device::Device(Application application)
	{
		// Setup dynamic library loader
		static vk::DynamicLoader dl;
		auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
		dispatchLoder_.init(vkGetInstanceProcAddr);

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
		dispatchLoder_.init(*instance_);

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
		throw std::runtime_error("Failed to select device!");

		// Find queue family
		auto queueFamilies = physicalDevice_.getQueueFamilyProperties();
		// Find Queue family index has graphics queue and present queue
		for (uint32_t i = 0; i < queueFamilies.size(); i++) {
			if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
				graphicsQueueFamilyIndex_ = i;
			}
		}
		throw std::runtime_error("No graphics queuefamily!");

		for (uint32_t i = 0; i < queueFamilies.size(); i++) {
			// Check selected physical device can handle the surface
			vk::Bool32 presentSupport = physicalDevice_.getSurfaceSupportKHR(i, surface_.get());
			if (presentSupport) {
				presentQueueFamilyIndex_ = i;
			}
		}
		throw std::runtime_error("No present queuefamily!");

		// NOTE : Fix to turn on compute queue only using async compute
		auto queueFamilies = physicalDevice_.getQueueFamilyProperties();
		// Find Queue family index has graphics queue and present queue
		for (uint32_t i = 0; i < queueFamilies.size(); i++) {
			if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eCompute) {
				computeQueueFamilyIndex_ = i;
			}
		}
		throw std::runtime_error("No compute queuefamily!");
		
		// Create logical device
		vector<vk::DeviceQueueCreateInfo> queueCreateInfos = {};
		set<uint32_t> queueFamilyIndices = { graphicsQueueFamilyIndex_, presentQueueFamilyIndex_, computeQueueFamilyIndex_ };
		float queuePriority = 1.0f;
		for (uint32_t queueFamilyIndice : queueFamilyIndices) {
			queueCreateInfos.push_back(
				vk::DeviceQueueCreateInfo{}
				.setQueueFamilyIndex(queueFamilyIndice)
				.setQueueCount(1)
				.setPQueuePriorities(&queuePriority)
			);
		}
		vk::DeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo
			.setPQueueCreateInfos(queueCreateInfos.data())
			.setQueueCreateInfoCount(queueCreateInfos.size()) // If you need multi queue when async compute, set mutivalue
			.setPpEnabledExtensionNames(requestInstanceExtensions_.data());
		// NOTE : FIX!
		vk::StructureChain<vk::DeviceCreateInfo/*, features*/> createInfoChain{
			deviceCreateInfo,
			//vk::PhysicalDevice
		};

		device_ = physicalDevice_.createDeviceUnique(createInfoChain.get<vk::DeviceCreateInfo>());
		dispatchLoder_.init(device_.get());

		graphicsQueue_ = device_->getQueue(graphicsQueueFamilyIndex_, 0);
		presentQueue_ = device_->getQueue(presentQueueFamilyIndex_, 0);
	}

	vk::PhysicalDevice Device::GetPhysicalDevice()
	{
		return physicalDevice_;
	}

	vk::Device Device::GetDevice()
	{
		return device_.get();
	}

	vk::SurfaceKHR Device::GetSurface()
	{
		return surface_.get();
	}
}