#pragma once

namespace sqrp
{
	class Buffer;
	class CommandBuffer;
	class DescriptorSet;
	class Device;
	class Fence;
	class FrameBuffer;
	class GUI;
	class Image;
	class Mesh;
	class Pipeline;
	class GraphicsPipeline;
	class ComputePipeline;
	class RenderPass;
	class Semaphore;
	class Shader;
	class Swapchain;

	using BufferHandle = std::shared_ptr<Buffer>;
	using CommandBufferHandle = std::shared_ptr<CommandBuffer>;
	using DescriptorSetHandle = std::shared_ptr<DescriptorSet>;
	using FenceHandle = std::shared_ptr<Fence>;
	using FrameBufferHandle = std::shared_ptr<FrameBuffer>;
	using GUIHandle = std::shared_ptr<GUI>;
	using ImageHandle = std::shared_ptr<Image>;
	using MeshHandle = std::shared_ptr<Mesh>;
	using GraphicsPipelineHandle = std::shared_ptr<GraphicsPipeline>;
	using ComputePipelineHandle = std::shared_ptr<ComputePipeline>;
	using PipelineHandle = std::shared_ptr<Pipeline>;
	using RenderPassHandle = std::shared_ptr<RenderPass>;
	using SemaphoreHandle = std::shared_ptr<Semaphore>;
	using ShaderHandle = std::shared_ptr<Shader>;
	using SwapchainHandle = std::shared_ptr<Swapchain>;
}