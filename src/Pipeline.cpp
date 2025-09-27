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
        const Device& device,
        RenderPassHandle pRenderPass,
        SwapchainHandle pSwapchain,
        ShaderHandle pVertexShader,
        ShaderHandle pPixelShader,
        DescriptorSetHandle pDescriptorSet
    )
        : pDevice_(&device)
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

        // 2. 固定機能ステート
		vk::VertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = vk::VertexInputRate::eVertex;

		std::array<vk::VertexInputAttributeDescription, 4> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = vk::Format::eR32G32B32A32Sfloat; // position
		attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32B32A32Sfloat; // position
        attributeDescriptions[1].offset = offsetof(Vertex, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = vk::Format::eR32G32B32A32Sfloat; // position
        attributeDescriptions[2].offset = offsetof(Vertex, tangent);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = vk::Format::eR32G32Sfloat; // position
        attributeDescriptions[3].offset = offsetof(Vertex, uv);

        vk::PipelineVertexInputStateCreateInfo vertexInput{};
        vertexInput.setVertexBindingDescriptionCount(1);
		vertexInput.setPVertexBindingDescriptions(&bindingDescription);
		vertexInput.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributeDescriptions.size()));
		vertexInput.setPVertexAttributeDescriptions(attributeDescriptions.data());
        // 頂点レイアウトは必要に応じて設定

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;

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
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        vk::PipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.polygonMode = vk::PolygonMode::eFill;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = vk::CullModeFlagBits::eBack;
        rasterizer.frontFace = vk::FrontFace::eClockwise;

        vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask =
            vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA;

        vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;

        // 3. パイプラインレイアウト
        vk::PipelineLayoutCreateInfo layoutInfo{};
		auto descriptorSetLayout = pDescriptorSet->GetDescriptorSetLayout();
        layoutInfo.setLayoutCount = 1;
        layoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayout_ = pDevice_->GetDevice().createPipelineLayoutUnique(layoutInfo);

        vk::PipelineDepthStencilStateCreateInfo depthStencilState{};
        depthStencilState
            .setDepthTestEnable(vk::True)
            .setDepthWriteEnable(vk::True)
            .setDepthCompareOp(vk::CompareOp::eLess)
            .setDepthBoundsTestEnable(vk::False)
            .setStencilTestEnable(vk::True);

        // 4. パイプライン作成
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
		pipelineInfo.setLayout(pipelineLayout_.get());
		pipelineInfo.setRenderPass(pRenderPass->GetRenderPass());
		pipelineInfo.setSubpass(0);

       /*auto result = pDevice_->GetDevice().createGraphicsPipeline(nullptr, pipelineInfo);
        if (result.result != vk::Result::eSuccess) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }
        pipeline_ = vk::UniquePipeline(result.value); */
        auto result = pDevice_->GetDevice().createGraphicsPipelinesUnique(nullptr, pipelineInfo);
        if (result.result != vk::Result::eSuccess) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }
		pipeline_ = std::move(result.value.front());
    }
}