#include "shader.h"


#define SHADERCOMPILE(h, x, y, n) \
Shader.m_Vaild[ n ] = Shader. h != VK_NULL_HANDLE; \
if (Shader.m_Vaild[ n ]) { \
	Shader. x = {}; \
	Shader. x .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO; \
	Shader. x .stage = y ; \
	Shader. x .module = Shader. h ; \
	Shader. x .pName = "main"; \
} \

void CleanCache(Aktion::Engine::AktionShader& Shader)
{
	if (Shader.m_VertexShader != VK_NULL_HANDLE) vkDestroyShaderModule(*Shader.m_Device, Shader.m_VertexShader, nullptr);
	if (Shader.m_FragmentShader != VK_NULL_HANDLE) vkDestroyShaderModule(*Shader.m_Device, Shader.m_FragmentShader, nullptr);
	if (Shader.m_GeometryShader != VK_NULL_HANDLE) vkDestroyShaderModule(*Shader.m_Device, Shader.m_GeometryShader, nullptr);
	if (Shader.m_ComputeShader != VK_NULL_HANDLE) vkDestroyShaderModule(*Shader.m_Device, Shader.m_ComputeShader, nullptr);
}


void Aktion::Engine::Shader_RegisterDevice(Aktion::Engine::AktionShader& Shader, VkDevice& _device)
{
	Shader.m_Device = &_device;
}
void Aktion::Engine::Shader_Clean(AktionShader& Shader)
{
}
bool Aktion::Engine::Shader_Compile(AktionShader& Shader)
{
	SHADERCOMPILE(m_VertexShader, m_VertexShaderStageInfo, VK_SHADER_STAGE_VERTEX_BIT, 0);
	SHADERCOMPILE(m_FragmentShader, m_FragmentShaderStageInfo, VK_SHADER_STAGE_FRAGMENT_BIT, 1);
	SHADERCOMPILE(m_GeometryShader, m_GeometryShaderStageInfo, VK_SHADER_STAGE_GEOMETRY_BIT, 2);
	SHADERCOMPILE(m_VertexShader, m_ComputeShaderStageInfo, VK_SHADER_STAGE_COMPUTE_BIT, 3);

	CleanCache(Shader);
	return true;
}
bool Aktion::Engine::Shader_RegisterCode(AktionShader& Shader, const char* code, Aktion::Engine::ShaderType type)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = sizeof(code);
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code);

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(*Shader.m_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		return false;
	}

	switch (type) {
	case Aktion::Engine::ShaderType::Vertex:
		Shader.m_VertexShader = shaderModule;
		break;
	case Aktion::Engine::ShaderType::Fragment:
		Shader.m_FragmentShader = shaderModule;
		break;
	case Aktion::Engine::ShaderType::Geometry:
		Shader.m_GeometryShader = shaderModule;
		break;
	case Aktion::Engine::ShaderType::Compute:
		Shader.m_ComputeShader = shaderModule;
		break;
	}

	return true;
}
void Aktion::Engine::Shader_Destroy(AktionShader& Shader) 
{
	CleanCache(Shader);
}
std::vector<VkPipelineRenderingCreateInfoKHR> Aktion::Engine::Shader_GetPipeline(AktionShader& Shader) 
{
	std::vector<VkPipelineRenderingCreateInfoKHR> result = std::vector<VkPipelineRenderingCreateInfoKHR>();
	//if (Shader.m_Vaild[0])result.push_back(m_VertexShaderStageInfo);
	return result;
}