#include "scene.h"
#include <system/config.h>
#include <system/gui.h>
#include <Imgui/utility/splitter.h>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

void Aktion::GUI::GUI_DrawPreviewWindow(AktionGUI& target, Engine::Engine& engine)
{
}
void Aktion::GUI::GUI_DrawOutputWindow(AktionGUI& target, Engine::Engine& engine)
{
}
void Aktion::GUI::GUI_InitScenePanel(AktionGUI& target, Engine::Engine& engine)
{
	target.m_Scene.m_ViewProjection = glm::perspective(72.0f, 16.0f / 9.0f, 0.01f, 1000.0f);
	target.m_Scene.m_ViewTransform = glm::mat4(1);
}
void Aktion::GUI::GUI_DrawScenePanel(AktionGUI& target, Engine::Engine& engine)
{
	

	if (ImGui::BeginChild(""))
	{

	}
	ImGui::EndChild();
}
