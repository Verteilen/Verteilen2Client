#include "menu.h"
#include <imgui.h>
#include <system/gui.h>

void M_NewProject()
{
}
void M_OpenProject()
{
}
void M_SaveProject()
{
}
void M_SaveProjectAs()
{
}
void M_Preference()
{
}
void M_ProjectSetting()
{
}
void M_Exit()
{
}
void M_Copy()
{
}
void M_Cut()
{
}
void M_Paste()
{
}
void M_Manual()
{
}
void M_Documentation()
{
}


void Aktion::GUI::GUI_DrawMainMenuBar(AktionGUI& target)
{
  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("New Project")) {
        M_NewProject();
      }
      if (ImGui::MenuItem("Open Project")) {
        M_OpenProject();
      }
      if (ImGui::BeginMenu("Open Recent Project")) {
        ImGui::EndMenu();
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Save Project")) {
        M_SaveProject();
      }
      if (ImGui::MenuItem("Save Project As...")) {
        M_SaveProjectAs();
      }
      ImGui::Separator();
      if (ImGui::BeginMenu("Import Asset")) {
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Export Asset")) {
        ImGui::EndMenu();
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Preference")) {
        M_Preference();
      }
      if (ImGui::MenuItem("Project Setting")) {
        M_ProjectSetting();
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Exit")) {
        M_Exit();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit")) {
      if (ImGui::MenuItem("Cut")) {
        M_Cut();
      }
      if (ImGui::MenuItem("Copy")) {
        M_Copy();
      }
      if (ImGui::MenuItem("Paste")) {
        M_Paste();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Window")) {
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Help")) {


      if (ImGui::MenuItem("Documentation")) {
        M_Documentation();
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Manual")) {
        M_Manual();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }

}
