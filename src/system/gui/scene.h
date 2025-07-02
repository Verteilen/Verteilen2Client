#pragma once
#include <glm/glm.hpp>

namespace Aktion::Engine
{
	typedef struct Engine;
}

namespace Aktion::GUI 
{
	typedef struct AktionGUI;

	struct SceneBuffer 
	{
		glm::mat4 m_ViewTransform;
		glm::mat4 m_ViewProjection;
	};

	void GUI_DrawPreviewWindow(AktionGUI& target, Engine::Engine& engine);
	void GUI_DrawOutputWindow(AktionGUI& target, Engine::Engine& engine);
	void GUI_InitScenePanel(AktionGUI& target, Engine::Engine& engine);
	void GUI_DrawScenePanel(AktionGUI& target, Engine::Engine& engine);
}