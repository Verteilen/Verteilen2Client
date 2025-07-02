#pragma once
#include <functional>
#include <system/config.h>
#include <imgui.h>

namespace Aktion::Engine 
{
  typedef struct Engine;
}

namespace Aktion::GUI {
	typedef struct AktionGUI;

  struct AktionSetting {
    ImGuiStyle m_Style;
    float m_WindowScale;
    float m_GlobalScale;
  };

  struct ThemeBuffer {
    std::function<void(ImGuiStyle*)> m_ThemeClassic;
    std::function<void(ImGuiStyle*)> m_ThemeDark;
    std::function<void(ImGuiStyle*)> m_ThemeWhite;
    int m_Current;
  };

  void GUI_InitSetting(AktionGUI& target);
	void GUI_DrawSettingView(AktionGUI& target, Engine::Engine& engine, ImGuiIO& io, ImGuiStyle* ref);
}