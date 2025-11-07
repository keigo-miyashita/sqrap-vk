#include "Pipeline.hpp"

#include "DescriptorSet.hpp"
#include "Device.hpp"
#include "Mesh.hpp"
#include "RenderPass.hpp"
#include "Shader.hpp"
#include "Swapchain.hpp"

using namespace std;

namespace sqrp
{
    Pipeline::Pipeline(
        const Device& device
    )
		: pDevice_(&device)
    {

    }

    vk::Pipeline Pipeline::GetPipeline() const
    {
        return pipeline_.get();
    }

    vk::PipelineLayout Pipeline::GetPipelineLayout() const
    {
        return pipelineLayout_.get();
    }

    GraphicsPipeline::GraphicsPipeline(
        const Device& device,
        RenderPassHandle pRenderPass,
        SwapchainHandle pSwapchain,
        ShaderHandle pVertexShader,
        ShaderHandle pPixelShader,
        DescriptorSetHandle pDescriptorSet,
        vk::PushConstantRange pushConstantRange,
        bool enableDepthWrite,
        bool needVertexBuffer
    )
        : Pipeline(device)
    {
        vk::PipelineShaderStageCreateInfo vertStage{};
        vertStage.stage = pVertexShader->GetShaderStage();
        vertStage.module = pVertexShader->GetShaderModule();
        vertStage.pName = "main";

        vk::PipelineShaderStageCreateInfo fragStage{};
        fragStage.stage = pPixelShader->GetShaderStage();
        fragStage.module = pPixelShader->GetShaderModule();
        fragStage.pName = "main";

        vk::PipelineShaderStageCreateInfo shaderStages[] = { vertStage, fragStage };

		// Vertex format
		vk::VertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = vk::VertexInputRate::eVertex;

		std::array<vk::VertexInputAttributeDescription, 4> attributeDescriptions{};

        // position
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = vk::Format::eR32G32B32A32Sfloat;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

        // normal
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32B32A32Sfloat;
        attributeDescriptions[1].offset = offsetof(Vertex, normal);

        // tangent
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = vk::Format::eR32G32B32A32Sfloat;
        attributeDescriptions[2].offset = offsetof(Vertex, tangent);

        // uv
        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = vk::Format::eR32G32Sfloat;
        attributeDescriptions[3].offset = offsetof(Vertex, uv);

        vk::PipelineVertexInputStateCreateInfo vertexInput{};
        if (needVertexBuffer) {
            vertexInput.setVertexBindingDescriptionCount(1);
            vertexInput.setPVertexBindingDescriptions(&bindingDescription);
            vertexInput.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributeDescriptions.size()));
            vertexInput.setPVertexAttributeDescriptions(attributeDescriptions.data());
        }
        else {
            vertexInput.setVertexBindingDescriptionCount(0);
            vertexInput.setVertexAttributeDescriptionCount(0);
        }

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;

        std::array<vk::DynamicState, 2> dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
		};
		vk::PipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.setDynamicStates(dynamicStates);

        vk::Viewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(pSwapchain->GetWidth());
        viewport.height = static_cast<float>(pSwapchain->GetHeight());
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vk::Rect2D scissor{ {0, 0}, {pSwapchain->GetWidth(), pSwapchain->GetHeight()}};
        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        vk::PipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.polygonMode = vk::PolygonMode::eFill;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = vk::CullModeFlagBits::eBack;
        rasterizer.frontFace = vk::FrontFace::eCounterClockwise;

        vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

        std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments(pRenderPass->GetNumColorAttachments());
        for (int i = 0; i < pRenderPass->GetNumColorAttachments(); i++) {
            colorBlendAttachments[i].colorWriteMask =
                vk::ColorComponentFlagBits::eR |
                vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB |
				vk::ColorComponentFlagBits::eA;
		}

        vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.attachmentCount = colorBlendAttachments.size();
        colorBlending.pAttachments = colorBlendAttachments.data();

		// pipeline layout
        vk::PipelineLayoutCreateInfo layoutInfo{};
		auto descriptorSetLayout = pDescriptorSet->GetDescriptorSetLayout();
        layoutInfo.setLayoutCount = 1;
        layoutInfo.pSetLayouts = &descriptorSetLayout;
		layoutInfo.setPushConstantRangeCount(pushConstantRange.size > 0 ? 1 : 0);
		layoutInfo.pPushConstantRanges = pushConstantRange.size > 0 ? &pushConstantRange : nullptr;
        pipelineLayout_ = pDevice_->GetDevice().createPipelineLayoutUnique(layoutInfo);

        vk::PipelineDepthStencilStateCreateInfo depthStencilState{};
        depthStencilState
            .setDepthTestEnable(vk::True)
            .setDepthWriteEnable(enableDepthWrite ? vk::True : vk::False)
            .setDepthCompareOp(vk::CompareOp::eLessOrEqual)
            .setDepthBoundsTestEnable(vk::False)
            .setStencilTestEnable(vk::False);

        // pipeline
        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.setStageCount(2);
		pipelineInfo.setPStages(shaderStages);
		pipelineInfo.setPDepthStencilState(&depthStencilState);
        pipelineInfo.setPVertexInputState(&vertexInput);
		pipelineInfo.setPInputAssemblyState(&inputAssembly);
		pipelineInfo.setPViewportState(&viewportState);
		pipelineInfo.setPRasterizationState(&rasterizer);
		pipelineInfo.setPMultisampleState(&multisampling);
		pipelineInfo.setPColorBlendState(&colorBlending);
		pipelineInfo.setPDynamicState(&dynamicState);
		pipelineInfo.setLayout(pipelineLayout_.get());
		pipelineInfo.setRenderPass(pRenderPass->GetRenderPass());
		pipelineInfo.setSubpass(0);

        auto result = pDevice_->GetDevice().createGraphicsPipelinesUnique(nullptr, pipelineInfo);
        if (result.result != vk::Result::eSuccess) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }
		pipeline_ = std::move(result.value.front());
    }

    ComputePipeline::ComputePipeline(
        const Device& device,
        ShaderHandle pComputeShader,
        DescriptorSetHandle pDescriptorSet,
        vk::PushConstantRange pushConstantRange
	) : Pipeline(device)
    {
        vk::PipelineShaderStageCreateInfo computeStage{};
        computeStage.stage = pComputeShader->GetShaderStage();
        computeStage.module = pComputeShader->GetShaderModule();
        computeStage.pName = "main";

		// pipeline layout
        vk::PipelineLayoutCreateInfo layoutInfo{};
        auto descriptorSetLayout = pDescriptorSet->GetDescriptorSetLayout();
        layoutInfo.setLayoutCount = 1;
        layoutInfo.pSetLayouts = &descriptorSetLayout;
        layoutInfo.setPushConstantRangeCount(pushConstantRange.size > 0 ? 1 : 0);
        layoutInfo.pPushConstantRanges = pushConstantRange.size > 0 ? &pushConstantRange : nullptr;
        pipelineLayout_ = pDevice_->GetDevice().createPipelineLayoutUnique(layoutInfo);

		// pipeline
        vk::ComputePipelineCreateInfo pipelineInfo{};
		pipelineInfo.setStage(computeStage);
		pipelineInfo.setLayout(pipelineLayout_.get());

        auto result = pDevice_->GetDevice().createComputePipelinesUnique(nullptr, pipelineInfo);
        if (result.result != vk::Result::eSuccess) {
            throw std::runtime_error("failed to create compute pipeline!");
        }
        pipeline_ = std::move(result.value.front());
    }
}