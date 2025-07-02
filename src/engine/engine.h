#pragma once
#include <vector>
#include <string>
#include <map>
#include <engine/render/shader.h>
#include <engine/render/model.h>
#include <engine/scene/scene_node.h>
#include <engine/time.h>
#include <engine/project.h>

namespace Aktion::Engine 
{
	enum EngineInitErrorCode 
	{
		None,
		PathNotFound
	};

	struct Engine 
	{
		ProjectInfo m_ProjectInfo;
		std::map<std::string, std::string> m_Parameters;
		AktionTime m_Time;
		RootSceneNode m_Root;
		std::vector<AktionModel> m_Model;
		std::vector<AktionShader> m_Shader;
	};

	EngineInitErrorCode Engine_Init(Engine& engine);
	void Engine_MainLoop(Engine& engine);
	void Engine_Reset(Engine& engine);

	void Engine_SaveProjectFile(Engine& engine);
	void Engine_LoadProjectFile(Engine& engine);
}