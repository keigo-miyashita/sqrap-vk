//#define VULKAN_HPP_DISPATCH_DYNAMIC 1
//
//#include "core.hpp"
//
//static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
//	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//	VkDebugUtilsMessageTypeFlagsEXT messageType,
//	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
//	void* pData)
//{
//	std::cerr << "validation layer : " << pCallbackData->pMessage << std::endl;
//
//	return VK_FALSE;
//}
//
//void Core::CreateInstance(uint32_t apiVersion, std::vector<const char*>& layers)
//{
//	static vk::DynamicLoader dl;
//	auto vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
//	vk::DispatchLoaderDynamic dispatchLoder;
//	dispatchLoder.init(vkGetInstanceProcAddr);
//
//	vk::ApplicationInfo appInfo = vk::ApplicationInfo()
//		.setPApplicationName("App")
//		.setApplicationVersion(VK_MAKE_API_VERSION(1, 0, 0, 0))
//		.setApiVersion(apiVersion);
//
//	SetRequiredExtensionForGLFW();
//	SetRequiredExtensionForDebug();
//
//	instance = vk::createInstanceUnique(vk::InstanceCreateInfo()
//		.setPApplicationInfo(&appInfo)
//		.setEnabledExtensionCount(requestInstanceExtensions.size())
//		.setPpEnabledExtensionNames(requestInstanceExtensions.data())
//		.setEnabledLayerCount(layers.size())
//		.setPpEnabledLayerNames(layers.data())
//	);
//	dispatchLoder.init(*instance);
//}
//
//void Core::CreatedebugMessenger(vk::Instance instance, GLFWwindow* window)
//{
//	vk::DebugUtilsMessengerCreateInfoEXT createInfo{};
//	createInfo.setMessageType(
//		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
//		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
//		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
//	createInfo.setMessageSeverity(
//		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
//		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
//	createInfo.setPfnUserCallback(&debugCallback);
//
//	debugMessenger = instance.createDebugUtilsMessengerEXTUnique(createInfo);
//}
//
////void Core::CreateSurface(GLFWwindow* window)
////{
////	VkSurfaceKHR surf;
////	if (glfwCreateWindowSurface(*instance, window, nullptr, surface.get()) != VK_SUCCESS) {
////		throw std::runtime_error("Failed to create window surface\n");
////	}
////
////	surface = vk::UniqueSurfaceKHR{vk::SurfaceKHR(surf), (*instance)};
////}