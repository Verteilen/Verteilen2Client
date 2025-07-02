#pragma once
#include <engine/engine.h>

namespace Aktion::GUI {
  typedef struct AktionGUI;

  struct LayoutBuffer 
  {
    float m_ResourcePanelTop;
    float m_ResourcePanelBottom;
    float m_ResourceLeft;
    float m_ResourceRight;

    float m_SceneInspectorRight;
    float m_SceneMainFrameLeft;
    float m_SceneLeftRow1;
    float m_SceneLeftRow2;
    float m_SceneLeftRow3;
    float m_ScenePreviewLeft;
    float m_SceneOutputRight;
  };

  void GUI_DrawMainPanels(AktionGUI& target, Engine::Engine& engine);
  LayoutBuffer GUI_GenerateLayoutBuffer();
}