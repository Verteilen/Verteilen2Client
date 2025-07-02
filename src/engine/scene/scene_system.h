#pragma once
#include <vector>
#include <engine/scene/scene_component.h>

namespace Aktion::Engine
{
	struct BaseSceneSystem 
	{
		bool isBuildIn;
		std::vector<SceneComponent> m_Components;
	};
	/// <summary>
	/// The system that will run through all component in the scene
	/// </summary>
	struct SceneSystem : public BaseSceneSystem
	{
		std::string m_Path;
		std::string m_TargetPath;
	};

	void SceneSystem_Run(BaseSceneSystem& system);
}