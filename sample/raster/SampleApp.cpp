#include "SampleApp.hpp"

using namespace std;
using namespace sqrp;

SampleApp::SampleApp(std::string appName, unsigned int windowWidth, unsigned int windowHeight)
	: Application(appName, windowWidth, windowHeight)
{

}

void SampleApp::OnStart()
{
	device_.Init(*this);

	swapchain_ = device_.CreateSwapchain(windowWidth_, windowHeight_);

	renderPass_ = device_.CreateRenderPass(swapchain_);

	depthImages_.resize(swapchain_->GetInflightCount());
	vk::SamplerCreateInfo depthSamplerInfo;
	depthSamplerInfo
		.setMagFilter(vk::Filter::eNearest)
		.setMinFilter(vk::Filter::eNearest)
		.setMipmapMode(vk::SamplerMipmapMode::eNearest)
		.setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
		.setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
		.setAddressModeW(vk::SamplerAddressMode::eClampToEdge)
		.setMipLodBias(0.0f)
		.setAnisotropyEnable(VK_FALSE)
		.setMaxAnisotropy(1.0f)
		.setCompareEnable(VK_FALSE)
		.setCompareOp(vk::CompareOp::eAlways)
		.setMinLod(0.0f)
		.setMaxLod(0.0f)
		.setBorderColor(vk::BorderColor::eIntOpaqueWhite)
		.setUnnormalizedCoordinates(VK_FALSE);

	for (int i = 0; i < swapchain_->GetInflightCount(); i++) {

		depthImages_[i] = device_.CreateImage(
			"Depth" + to_string(i),
			vk::Extent3D{ windowWidth_, windowHeight_, 1 },
			vk::ImageType::e2D,
			vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
			vk::Format::eD32Sfloat,
			vk::ImageLayout::eUndefined,
			vk::ImageAspectFlagBits::eDepth,
			1,
			1,
			vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
			depthSamplerInfo
		);
	}

	frameBuffer_ = device_.CreateFrameBuffer(renderPass_, swapchain_, depthImages_);

	mesh_ = device_.CreateMesh(string(MODEL_DIR) + "Suzanne.gltf");

	// Camera
	camera_.Init((float)GetWindowWidth() / (float)GetWindowHeight(), glm::vec3(0.0f, 0.0f, 5.0f)); // Note sign

	// Light
	light0_.pos = glm::vec4(10.0f, 10.0f, -5.0f, 1.0f);
	light0_.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// Sphere0
	object_ = {};
	/*XMMATRIX modelMat = XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(180)), XMMatrixIdentity());
	sphere0_.model = modelMat;
	sphere0_.invTransModel = XMMatrixTranspose(XMMatrixInverse(nullptr, sphere0_.model));*/

	cameraBuffer_ = device_.CreateBuffer(sizeof(CameraMatrix), vk::BufferUsageFlagBits::eUniformBuffer, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
	cameraBuffer_->Write(CameraMatrix{ camera_.GetView(), camera_.GetProj() });
	
	objectBuffer_ = device_.CreateBuffer(sizeof(TransformMatrix), vk::BufferUsageFlagBits::eUniformBuffer, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
	objectBuffer_->Write(object_);

	lightBuffer_ = device_.CreateBuffer(sizeof(Light), vk::BufferUsageFlagBits::eUniformBuffer, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
	lightBuffer_->Write(light0_);

	colorBuffer_ = device_.CreateBuffer(sizeof(glm::vec4), vk::BufferUsageFlagBits::eUniformBuffer, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_HOST);
	colorBuffer_->Write(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	descriptorSet_ = device_.CreateDescriptorSet({
		{ cameraBuffer_, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment },
		{ objectBuffer_, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment },
		{ lightBuffer_, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment },
		{ colorBuffer_, vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment }
		}
	);

	vertShader_ = device_.CreateShader(compiler_, string(SHADER_DIR) + "Lambert.shader", sqrp::ShaderType::Vertex);
	pixelShader_ = device_.CreateShader(compiler_, string(SHADER_DIR) + "Lambert.shader", sqrp::ShaderType::Pixel);

	pipeline_ = device_.CreateGraphicsPipeline(renderPass_, swapchain_, vertShader_, pixelShader_, descriptorSet_);
}

void SampleApp::OnUpdate()
{
	camera_.Update(windowWidth_, windowHeight_);
	cameraBuffer_->Write(CameraMatrix{ camera_.GetView(), camera_.GetProj() });

	swapchain_->WaitFrame();

	auto& commandBuffer = swapchain_->GetCurrentCommandBuffer();
	uint32_t infligtIndex = swapchain_->GetCurrentInflightIndex();

	commandBuffer->Begin();

	//commandBuffer->TransitionLayout(swapchain_->GetCurrentImage(), vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal);

	commandBuffer->BeginRenderPass(renderPass_, frameBuffer_, infligtIndex);

	commandBuffer->SetViewport(swapchain_->GetWidth(), swapchain_->GetHeight());
	commandBuffer->SetScissor(swapchain_->GetWidth(), swapchain_->GetHeight());
	commandBuffer->BindPipeline(pipeline_, vk::PipelineBindPoint::eGraphics);
	commandBuffer->BindDescriptorSet(pipeline_, descriptorSet_, vk::PipelineBindPoint::eGraphics);
	commandBuffer->BindMeshBuffer(mesh_);
	commandBuffer->DrawMesh(mesh_, mesh_->GetNumIndices());

	commandBuffer->EndRenderPass();

	//commandBuffer->TransitionLayout(swapchain_->GetCurrentImage(), vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);

	commandBuffer->End();

	device_.Submit(
		QueueContextType::General, commandBuffer, vk::PipelineStageFlagBits::eColorAttachmentOutput,
		swapchain_->GetImageAcquireSemaphore()->GetSemaphore(), swapchain_->GetRenderCompleteSemaphore()->GetSemaphore(), swapchain_->GetCurrentFence()
	);

	swapchain_->Present();
}

void SampleApp::OnResize(unsigned int width, unsigned int height)
{
	if (width == 0 || height == 0) return;
	device_.WaitIdle(QueueContextType::General);
	swapchain_->Recreate(width, height);
	frameBuffer_->Recreate(width, height);
}

void SampleApp::OnTerminate()
{
	device_.WaitIdle(QueueContextType::General);
}