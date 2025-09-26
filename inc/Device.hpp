#pragma once

#include "pch.hpp"

#include "Alias.hpp"

#include "Application.hpp"

namespace sqrp
{
	class Application;
	class Buffer;
	class Fence;
	class FrameBuffer;
	class Image;
	class Mesh;
	class RenderPass;
	class Semaphore;
	class Swawpchain;

	struct Vertex;

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
		std::map<QueueContextType, QueueContext> queueContexts_;
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
		VmaAllocator allocator_;

		bool isDeviceExtensionSupport(vk::PhysicalDevice physDev);
		bool isDeviceSuitable(vk::PhysicalDevice physDev);

	public:
		Device();
		~Device() = default;
		bool Init(Application application);
		BufferHandle CreateBuffer(
			int size,
			vk::BufferUsageFlags usage,
			VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
			VmaAllocationCreateFlags allocationFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
		) const;
		CommandBufferHandle CreateCommandBuffer(const Device& device, QueueContextType queueType = QueueContextType::General) const;
		FenceHandle CreateFence(bool signal = true) const;
		FrameBufferHandle CreateFrameBuffer(SwapchainHandle pSwapchain, int numAttachmentBuffers0) const;
		ImageHandle CreateImage(
			vk::Extent3D extent3D = vk::Extent3D{ 512, 512, 1 },
			vk::ImageType imageType = vk::ImageType::e2D,
			vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eSampled,
			vk::Format format = vk::Format::eR8G8B8A8Srgb,
			int mipLevels = 1,
			int arrayLayers = 1,
			vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1,
			vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
			vk::SamplerCreateInfo samplerCreateInfo = {}
		)  const;
		MeshHandle CreateMesh(std::string modelPath) const;
		MeshHandle CreateMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indicesh) const;
		RenderPassHandle CreateRenderPass(SwapchainHandle pSwapchain) const;
		SemaphoreHandle CreateSemaphore() const;
		SwapchainHandle CreateSwapchain(uint32_t width, uint32_t height) const;

		void Submit(
			QueueContextType type,
			CommandBufferHandle pCommandBuffer,
			const std::vector<vk::PipelineStageFlags>& waitDstStageMasks,
			const std::vector<vk::Semaphore>& pWaitSemaphores,
			const std::vector<vk::Semaphore>& pSignalSemaphores,
			FenceHandle pFence = nullptr
		) const;
		void Submit(
			QueueContextType type,
			CommandBufferHandle pCommandBuffer,
			vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eNone,
			SemaphoreHandle pWaitSemaphore = nullptr,
			SemaphoreHandle pSignalSemaphore = nullptr,
			FenceHandle pFence = nullptr
		) const;
		void WaitIdle(QueueContextType type) const;

		void OneTimeSubmit(std::function<void(CommandBufferHandle pCommandBuffer)>&& command) const;
		void SetObjectName(uint64_t object, vk::ObjectType objectType, const std::string& name) const;

		VmaAllocator GetAllocator() const;
		vk::PhysicalDevice GetPhysicalDevice() const;
		vk::Device GetDevice() const;
		vk::SurfaceKHR GetSurface() const;
		const std::map<QueueContextType, QueueContext>& GetQueueContexts() const;
		//uint32_t GetPresentQueueFamilyIndex() const;
	};
}