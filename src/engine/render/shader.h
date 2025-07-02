#pragma once
#include <vector>
#include <utility/dependencyInclude.h>

namespace Aktion::Engine {

	struct AktionShader {
		VkDevice* m_Device;
		VkShaderModule m_VertexShader = VK_NULL_HANDLE;
		VkShaderModule m_FragmentShader = VK_NULL_HANDLE;
		VkShaderModule m_GeometryShader = VK_NULL_HANDLE;
		VkShaderModule m_ComputeShader = VK_NULL_HANDLE;

		VkPipelineShaderStageCreateInfo m_VertexShaderStageInfo = {};
		VkPipelineShaderStageCreateInfo m_FragmentShaderStageInfo = {};
		VkPipelineShaderStageCreateInfo m_GeometryShaderStageInfo = {};
		VkPipelineShaderStageCreateInfo m_ComputeShaderStageInfo = {};

		VkPipelineRenderingCreateInfoKHR m_ShaderStages[4];
		bool m_Vaild[4];
	};

	enum class ShaderType {
		Vertex,
		Fragment,
		Geometry,
		Compute
	};

	void Shader_RegisterDevice(AktionShader& Shader, VkDevice& _device);
	void Shader_Clean(AktionShader& Shader);
	bool Shader_Compile(AktionShader& Shader);
	bool Shader_RegisterCode(AktionShader& Shader, const char* code, ShaderType type);
	void Shader_Destroy(AktionShader& Shader);
	std::vector<VkPipelineRenderingCreateInfoKHR> Shader_GetPipeline(AktionShader& Shader);
}