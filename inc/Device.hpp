#pragma once

#include "pch.hpp"

#include "Alias.hpp"

#include "Application.hpp"
#include "Compiler.hpp"
#include "DescriptorSet.hpp"
#include "FrameBuffer.hpp"
#include "Mesh.hpp"
#include "RenderPass.hpp"

namespace sqrp
{
	class Application;
	class Buffer;
	class CommandBuffer;
	class DescriptorSet;
	class Fence;
	class Image;
	class Pipeline;
	class Semaphore;
	class Shader;
	class Swapchain;

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
		bool isSupportRayTracing_ = false;
		vk::PhysicalDevice physicalDevice_;
		vk::UniqueDevice device_;
		vk::UniqueDebugUtilsMessengerEXT debugMessenger_;
		vk::UniqueSurfaceKHR surface_;
		std::map<QueueContextType, QueueContext> queueContexts_;
		VmaAllocator allocator_;

		bool isDeviceExtensionSupport(vk::PhysicalDevice physDev);
		bool isDeviceSuitable(vk::PhysicalDevice physDev);
		bool isDeviceRayTracingSupport(vk::PhysicalDevice physDev);

	public:
		Device();
		~Device();
		bool Init(Application application);
		BufferHandle CreateBuffer(
			int size,
			vk::BufferUsageFlags usage,
			VmaAllocationCreateFlags allocationFlags,
			VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
		) const;
		CommandBufferHandle CreateCommandBuffer(QueueContextType queueType = QueueContextType::General) const;
		DescriptorSetHandle CreateDescriptorSet(std::vector<DescriptorSetCreateInfo> descriptorSetCreateInfos) const;
		FenceHandle CreateFence(bool signal = true) const;
		FrameBufferHandle CreateFrameBuffer(RenderPassHandle pRenderPass, SwapchainHandle pSwapchain, bool useDepth = true) const;
		FrameBufferHandle CreateFrameBuffer(RenderPassHandle pRenderPass, std::vector<FrameBufferInfo> frameBufferInfos, uint32_t width, uint32_t height, int infligtCount, SwapchainHandle pSwapchain = nullptr) const;
		GUIHandle CreateGUI(GLFWwindow* window, SwapchainHandle pSwapchain, RenderPassHandle pRenderPass) const;
		ImageHandle CreateImage(
			std::string name = "Image",
			vk::Extent3D extent3D = vk::Extent3D{ 512, 512, 1 },
			vk::ImageType imageType = vk::ImageType::e2D,
			vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eSampled,
			vk::Format format = vk::Format::eR8G8B8A8Srgb,
			vk::ImageLayout imageLayout = vk::ImageLayout::eUndefined,
			vk::ImageAspectFlags aspectFlags = vk::ImageAspectFlagBits::eColor,
			int mipLevels = 1,
			int arrayLayers = 1,
			vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1,
			vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
			vk::SamplerCreateInfo samplerCreateInfo = {}
		)  const;
		MeshHandle CreateMesh(std::string modelPath) const;
		MeshHandle CreateMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) const;
		PipelineHandle CreatePipeline(
			RenderPassHandle pRenderPass,
			SwapchainHandle pSwapchain,
			ShaderHandle pVertexShader,
			ShaderHandle pPixelShader,
			DescriptorSetHandle pDescriptorSet
		) const;
		RenderPassHandle CreateRenderPass(SwapchainHandle pSwapchain) const;
		RenderPassHandle CreateRenderPass(std::vector<SubPassInfo> subPassInfos, std::map<std::string, AttachmentInfo> attachmentNameToInfo) const;
		SemaphoreHandle CreateSemaphore() const;
		ShaderHandle CreateShader(const Compiler& compiler, const std::string& fileName, ShaderType shaderType) const;
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
		vk::Instance GetInstance() const;
		vk::SurfaceKHR GetSurface() const;
		const std::map<QueueContextType, QueueContext>& GetQueueContexts() const;
		vk::Queue GetQueue(QueueContextType type) const;
	};
}