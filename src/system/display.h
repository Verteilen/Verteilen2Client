#pragma once
#include <interface/interface_display_manager.h>
#include <utility/dependencyInclude.h>

namespace Aktion::System {
	/// <summary>
	/// The presentation display window
	/// </summary>
	struct AktionDisplay
	{
		bool m_Running = true;
		int32_t m_Width = 0;
		int32_t m_Height = 0;
		VkResult m_VKError = VK_SUCCESS;
		VkInstance m_VKInst = VK_NULL_HANDLE;
		VkPhysicalDevice m_VKPhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_VKDevice = VK_NULL_HANDLE;
		VkSurfaceKHR m_VKSurface = VK_NULL_HANDLE;
		VkQueue m_VKGraphicsQueue = VK_NULL_HANDLE;
		uint32_t m_VKGraphicsQueueIndex = UINT32_MAX;
		VkPipelineCache m_VKPipeline = VK_NULL_HANDLE;
		VkDescriptorPool m_VKDescriptor = VK_NULL_HANDLE;
		VkAllocationCallbacks* m_VKAllocator = nullptr;
		VkDebugReportCallbackEXT m_VKDebugReport = VK_NULL_HANDLE;

		SDL_Window* m_SDLWin = nullptr;
	};

	void Display_Destroy(AktionDisplay& display);
	void Display_Init(AktionDisplay& display);
	bool Display_IsRunning(AktionDisplay& display);

	void Display_MainLoop(AktionDisplay& display);
}