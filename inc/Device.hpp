#pragma once

#include "pch.hpp"

#include "Application.hpp"
#include "Swapchain.hpp"

#include "Alias.hpp"

namespace sqrp
{
	class Application;
	class Swawpchain;

	class Device
	{
	private:
		vk::UniqueInstance instance_;
		//vk::DispatchLoaderDynamic dispatchLoader_;
		std::vector<const char*> layers_ = {};
		std::vector<const char*> requestInstanceExtensions_ = {};
		std::vector<const char*> requestDeviceExtensions_ = {};
		vk::PhysicalDevice physicalDevice_;
		vk::UniqueDevice device_;
		uint32_t computeQueueFamilyIndex_ = -1;
		uint32_t graphicsQueueFamilyIndex_ = -1;
		uint32_t presentQueueFamilyIndex_ = -1;
		vk::UniqueDebugUtilsMessengerEXT debugMessenger_;
		vk::UniqueSurfaceKHR surface_;
		vk::Queue computeQueue_;
		vk::UniqueCommandPool computeCommandPool_;
		vk::Queue graphicsQueue_;
		vk::UniqueCommandPool graphicsCommandPool_;
		vk::Queue presentQueue_;
		vk::UniqueCommandPool presentCommandPool_;

		bool isDeviceExtensionSupport(vk::PhysicalDevice physDev);
		bool isDeviceSuitable(vk::PhysicalDevice physDev);

	public:
		Device();
		~Device() = default;
		bool Init(Application application);
		SwapchainHandle CreateSwapchain(uint32_t width, uint32_t height);
		vk::PhysicalDevice GetPhysicalDevice() const;
		vk::Device GetDevice() const;
		vk::SurfaceKHR GetSurface() const;
		uint32_t GetGraphicsQueueFamilyIndex() const;
		uint32_t GetPresentQueueFamilyIndex() const;
	};
}