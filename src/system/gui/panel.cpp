#include "panel.h"
#include <system/config.h>
#include <filesystem>
#include <imgui.h>
//
// Imgui and plugin
//
#include <imgui_node_editor.h>
#include <Imgui/utility/splitter.h>
//
// Engine lib
//
#include <system/gui.h>
#include <system/gui/logic.h>
#include <system/gui/resource.h>
#include <system/gui/comp.h>

void T_Resource(Aktion::GUI::AktionGUI& target, Aktion::Engine::Engine& engine, ImGuiIO& io)
{
  Aktion::GUI::Splitter(false, 4.0f, &target.m_Layout.m_ResourcePanelTop, &target.m_Layout.m_ResourcePanelBottom, 300.0f, 300.0f);
  Aktion::GUI::GUI_DrawResourceContent(target);
  Aktion::GUI::GUI_DrawResourcesView(target, engine.m_ProjectInfo);
}
void T_Scene()
{
  ImGui::Text("Scene");
}
void T_Logic(Aktion::GUI::AktionGUI& target, ImGuiIO& io)
{
  ax::NodeEditor::SetCurrentEditor(target.m_Logic.m_EditorContext);
  ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);
  ImGui::BeginHorizontal("Login Button Control Row");

  bool ButtonEvent = ImGui::Button("Create Node");
  bool HotKeyEvent = (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_A, false));

  if (ButtonEvent || HotKeyEvent) 
  {
    target.m_Logic.m_ShowAddNode = true;
    target.m_Logic.m_CursorLastPos = ImGui::GetMousePos();
  }

  if (target.m_Logic.m_ShowAddNode) 
  {
    ImGui::OpenPopup("Create Logic Node");
  }
  Aktion::GUI::GUI_Logic_PopupAddNode(target, target.m_Logic);

  if (ImGui::Button("Zoom Out"))
    ax::NodeEditor::NavigateToContent();
  ImGui::EndHorizontal();

  Aktion::GUI::GUI_Logic_DrawLogicEditor(target);
  
  ax::NodeEditor::SetCurrentEditor(nullptr);
}
void T_Animation(Aktion::GUI::AktionGUI& target, ImGuiIO& io)
{

}
void T_Comp(Aktion::GUI::AktionGUI& target, Aktion::Engine::Engine& engine, ImGuiIO& io)
{
  ax::NodeEditor::SetCurrentEditor(target.m_Comp.m_EditorContext);
  ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);
  ImGui::BeginHorizontal("Comp Button Control Row");

  bool ButtonEvent = ImGui::Button("Create Node");
  bool HotKeyEvent = (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_A, false));

  if (ButtonEvent || HotKeyEvent) {
    target.m_Comp.m_ShowAddNode = true;
    target.m_Comp.m_CursorLastPos = ImGui::GetMousePos();
  }

  if (target.m_Comp.m_ShowAddNode)
  {
    ImGui::OpenPopup("Create Comp Node");
  }
  Aktion::GUI::GUI_Comp_PopupAddNode(target, target.m_Comp);

  if (ImGui::Button("Zoom Out"))
    ax::NodeEditor::NavigateToContent();
  ImGui::EndHorizontal();

  Aktion::GUI::GUI_Comp_DrawNodeEditor(target);
  ax::NodeEditor::SetCurrentEditor(nullptr);
}
void T_Setting(Aktion::GUI::AktionGUI& target, Aktion::Engine::Engine& engine, ImGuiIO& io, ImGuiStyle* ref)
{
  Aktion::GUI::GUI_DrawSettingView(target, engine, io, ref);
}

void Aktion::GUI::GUI_DrawMainPanels(AktionGUI& target, Engine::Engine& engine)
{
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  if (ImGui::BeginTabBar("MainFrame#Tabbar")) {
    if (ImGui::BeginTabItem("Resource")) {
      T_Resource(target, engine, io);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Scene")) {
      T_Scene();
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Logic")) {
      T_Logic(target, io);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Animation")) {
      T_Animation(target, io);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Composition")) {
      T_Comp(target, engine, io);
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Setting")) {
      T_Setting(target, engine, io, 0);
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
}

Aktion::GUI::LayoutBuffer Aktion::GUI::GUI_GenerateLayoutBuffer()
{
  return {
    700.0f,
    400.0f,
    300.0f,
    800.0f,

    300.0f,
    700.0f,
    550.0f,
    400.0f,
    100.0f,
    250.0f,
    250.0f
  };
}
