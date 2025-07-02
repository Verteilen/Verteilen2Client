#pragma once
//
//  Custom ImGUI implementation includes
//
#include "gui.h"
#include <system/display.h>
#include <utility/dependencyInclude.h>
#include <interface/interface_display_manager.h>


namespace Aktion::System {
	typedef uint32_t (*GetMinImageCount)();
	typedef bool (*GetSwapChainRebuild)();
	typedef void (*SetSwapChainRebuild)(bool v);
	typedef void (*SaveSettingFunc)(GUI::AktionSetting v);
	typedef void (*LoadSettingFunc)(GUI::SetSetting v);
	typedef struct AktionSystem;

	static SDL_Window* m_CurrentWindow;

	/// <summary>
	/// The node editor control window
	/// </summary>
	struct AktionWindow : public AktionDisplay
	{
		ImGui_ImplVulkanH_Window m_MainWindowData;
		GUI::AktionGUI m_GUI;

		InterfaceDisplayManager m_DisplayManager;
		uint32_t m_ExtensionCount = 0;
		uint32_t m_PhysicalDeviceCount = 0;
	};

	struct WindowContext {
		GetMinImageCount m_GetMinImageCount;
		GetSwapChainRebuild m_GetSwapChainRebuild;
		SetSwapChainRebuild m_SetSwapChainRebuild;
		SaveSettingFunc m_SaveSettingFunc;
		LoadSettingFunc m_LoadSettingFunc;
	};

	void DestroyWindow(AktionWindow& target);
	void Window_Init(AktionWindow& target);
	void Window_RegisterDisplayManager(AktionWindow& target, InterfaceDisplayManager manager);
	bool Window_IsRunning(AktionWindow& target);
	/// <summary>
	/// Called by AktionSystem. <br />
	/// This will pass one mainloop cycle for this window.
	/// </summary>
	/// <returns></returns>
	void Window_MainLoop(AktionWindow& target, AktionSystem& engine);

}