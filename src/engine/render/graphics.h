#pragma once
#include <utility/vulkanHelper.h>


namespace Aktion::Engine {
	struct AktionGraphicsPipeline {
		VkViewport m_Viewport;
		VkRect2D m_Scissor;
		VkFence m_Fence;
		VkDevice m_Device;
		VkPipelineLayout m_PipelineLayout;
	};

	struct AktionGraphicsInfo {
		uint32_t m_FrameCount;
		float m_DeltaTime;
	};

	void ApplyGraphicsPipeline(AktionGraphicsPipeline& pipeline);
	void ApplyViewport(AktionGraphicsPipeline& pipeline, float x = 0, float y = 0, float width = 1, float height = 1);
	void ApplyScissor(AktionGraphicsPipeline& pipeline, int32_t offset_x = 0, int32_t offset_y = 0, uint32_t extend_x = 1, uint32_t extend_y = 1);
	void DestroyPipeline(AktionGraphicsPipeline& pipeline);
	void RecreateSwapChain(AktionGraphicsPipeline& pipeline);
}