#include "graphics.h"


VkPipelineViewportStateCreateInfo viewport_apply(Aktion::Engine::AktionGraphicsPipeline& pipeline)
{
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &pipeline.m_Viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &pipeline.m_Scissor;
	return viewportState;
}
VkPipelineRasterizationStateCreateInfo rasterization_apply(Aktion::Engine::AktionGraphicsPipeline& pipeline) 
{
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
	return rasterizer;
}
VkPipelineMultisampleStateCreateInfo multisample_apply(Aktion::Engine::AktionGraphicsPipeline& pipeline) 
{
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional
	return multisampling;
}
VkPipelineColorBlendAttachmentState colorblend_apply(Aktion::Engine::AktionGraphicsPipeline& pipeline) 
{
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	return colorBlendAttachment;
}
void pipeline_layout(Aktion::Engine::AktionGraphicsPipeline& pipeline) {
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0; // Optional
	pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

	if (vkCreatePipelineLayout(pipeline.m_Device, &pipelineLayoutInfo, nullptr, &pipeline.m_PipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}


void Aktion::Engine::ApplyGraphicsPipeline(AktionGraphicsPipeline& pipeline) 
{
	VkPipelineViewportStateCreateInfo viewport = viewport_apply(pipeline);
	VkPipelineRasterizationStateCreateInfo rest = rasterization_apply(pipeline);
	VkPipelineMultisampleStateCreateInfo multi = multisample_apply(pipeline);
	VkPipelineColorBlendAttachmentState colorblend = colorblend_apply(pipeline);
	pipeline_layout(pipeline);
}
void Aktion::Engine::ApplyViewport(AktionGraphicsPipeline& pipeline, float x, float y, float width, float height)
{
	pipeline.m_Viewport.x = x;
	pipeline.m_Viewport.y = y;
	pipeline.m_Viewport.width = width;
	pipeline.m_Viewport.height = height;
}
void Aktion::Engine::ApplyScissor(AktionGraphicsPipeline& pipeline, int32_t offset_x, int32_t offset_y, uint32_t extend_x, uint32_t extend_y)
{
	pipeline.m_Scissor.offset = { offset_x, offset_y };
	pipeline.m_Scissor.extent = { extend_x, extend_y };
}
void Aktion::Engine::DestroyPipeline(AktionGraphicsPipeline& pipeline)
{
	vkDestroyPipelineLayout(pipeline.m_Device, pipeline.m_PipelineLayout, nullptr);
}

void Aktion::Engine::RecreateSwapChain(AktionGraphicsPipeline& pipeline)
{
	vkDeviceWaitIdle(pipeline.m_Device);
}
