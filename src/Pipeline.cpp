#include "Pipeline.hpp"

#include "DescriptorSet.hpp"
#include "Device.hpp"
#include "RenderPass.hpp"
#include "Shader.hpp"
#include "Swapchain.hpp"

using namespace std;

namespace sqrp
{
    Pipeline::Pipeline(
        const Device device,
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
        vk::PipelineVertexInputStateCreateInfo vertexInput{};
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

        // 4. パイプライン作成
        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInput;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.layout = pipelineLayout_.get();
        pipelineInfo.renderPass = pRenderPass->GetRenderPass();
        pipelineInfo.subpass = 0;

        auto result = pDevice_->GetDevice().createGraphicsPipeline(nullptr, pipelineInfo);
        if (result.result != vk::Result::eSuccess) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }
        pipeline_ = vk::UniquePipeline(result.value);
    }
}