#pragma once
#include <system/config.h>
//
// Make imgui imvec can be use math operaetors
//
#include <map>
#include <functional>
//
//  Windows handler includes
//
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
//
// Imgui and plugin
//
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_sdl3.h>
//
// Imgui and plugin
//
#include <system/gui/logic.h>
#include <system/gui/setting.h>
#include <system/gui/resource.h>
#include <system/gui/scene.h>
#include <system/gui/panel.h>

namespace Aktion::System 
{
  typedef struct WindowContext;
  typedef struct AktionSystem;
}

namespace Aktion::GUI {
  typedef struct AktionSetting;
  typedef std::function<void(AktionSetting)> SetSetting;

	struct AktionGUI {
		ImGui_ImplVulkan_InitInfo m_InitInfo = {};
    ImGui_ImplVulkanH_Window* wd = nullptr;
    ImGuiContext* m_GuiContext = nullptr;

    NodeEditorBuffer m_Logic;
    NodeEditorBuffer m_Comp;
    AktionResource m_Resource;
    ThemeBuffer m_Theme;
    LayoutBuffer m_Layout;
    SceneBuffer m_Scene;

    System::WindowContext* m_WindowContext = nullptr;
    AktionSetting m_Setting;
    float window_scale = 1.0f;
	};

  int GUI_GetTextureWidth(ImTextureID texture);
  int GUI_GetTextureHeight(ImTextureID texture);

  void GUI_Focus(AktionGUI& target);
	void GUI_RegisterWindowContext(System::WindowContext* winContext);
	void GUI_Init(AktionGUI& target, SDL_Window* win, ImGui_ImplVulkan_InitInfo info, ImGui_ImplVulkanH_Window* _wd);
	bool GUI_Update(AktionGUI& target, System::AktionSystem& system);
  void GUI_Destroy(AktionGUI& target);
}