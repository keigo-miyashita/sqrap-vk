#pragma once

#include "pch.hpp"

#include "Alias.hpp"

#include "Application.hpp"

namespace sqrp
{
	class Application;
	class Fence;
	class Semaphore;
	class Swawpchain;

	enum class QueueContextType
	{
		General, Graphics, Compute, Transfer, Present
	};

	struct QueueContext {
		uint32_t queueFamilyIndex = -1;
		vk::Queue queue;
		uint32_t queueIndex = -1;
		vk::UniqueCommandPool commandPool;
	};

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
		vk::UniqueDebugUtilsMessengerEXT debugMessenger_;
		vk::UniqueSurfaceKHR surface_;
		std::unordered_map<QueueContextType, QueueContext> queueContexts_;
		/*QueueContext computeQueueContext_;
		QueueContext graphicsQueueContext_;
		QueueContext computeQueueContext_;
		uint32_t computeQueueFamilyIndex_ = -1;
		uint32_t graphicsQueueFamilyIndex_ = -1;
		uint32_t presentQueueFamilyIndex_ = -1;
		vk::Queue computeQueue_;
		vk::UniqueCommandPool computeCommandPool_;
		vk::Queue graphicsQueue_;
		vk::UniqueCommandPool graphicsCommandPool_;
		vk::Queue presentQueue_;
		vk::UniqueCommandPool presentCommandPool_;*/

		bool isDeviceExtensionSupport(vk::PhysicalDevice physDev);
		bool isDeviceSuitable(vk::PhysicalDevice physDev);

	public:
		Device();
		~Device() = default;
		bool Init(Application application);
		FenceHandle CreateFence(bool signal = true);
		SemaphoreHandle CreateSemaphore();
		SwapchainHandle CreateSwapchain(uint32_t width, uint32_t height);
		vk::PhysicalDevice GetPhysicalDevice() const;
		vk::Device GetDevice() const;
		vk::SurfaceKHR GetSurface() const;
		const std::unordered_map<QueueContextType, QueueContext>& GetQueueContexts() const;
		//uint32_t GetPresentQueueFamilyIndex() const;
	};
}