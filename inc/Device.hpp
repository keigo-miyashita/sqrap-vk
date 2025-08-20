#pragma once

#include "pch.hpp"

#include "Application.hpp"

namespace sqrp
{
	class Application;

	class Device
	{
	private:
		vk::UniqueInstance instance_;
		vk::DispatchLoaderDynamic dispatchLoder_;
		std::vector<const char*> layers_ = {};
		std::vector<const char*> requestInstanceExtensions_ = {};
		std::vector<const char*> requestDeviceExtensions_ = {};
		vk::PhysicalDevice physicalDevice_;
		vk::UniqueDevice device_;
		uint32_t computeQueueFamilyIndex_ = {};
		uint32_t graphicsQueueFamilyIndex_ = {};
		uint32_t presentQueueFamilyIndex_ = {};
		vk::UniqueDebugUtilsMessengerEXT debugMessenger_;
		vk::UniqueSurfaceKHR surface_;
		vk::Queue computeQueue_;
		vk::Queue graphicsQueue_;
		vk::Queue presentQueue_;

		bool isDeviceExtensionSupport(vk::PhysicalDevice physDev);
		bool isDeviceSuitable(vk::PhysicalDevice physDev);

	public:
		Device(Application application);
		~Device() = default;
		vk::PhysicalDevice GetPhysicalDevice();
		vk::Device GetDevice();
		vk::SurfaceKHR GetSurface();
		uint32_t GetGraphicsQueueFamilyIndex();
		uint32_t GetPresentQueueFamilyIndex();
	};
}