#pragma once
#include <string>
#include <vector>
#include <engine/scene/scene_component.h>


namespace Aktion::Engine 
{
	typedef struct SceneNode;

	/// <summary>
	/// A node is like gameobject for unity.
	/// Each node can have multiple components
	/// </summary>
	struct SceneNode 
	{
		std::string m_Name;
		std::vector<SceneComponent> m_Components;
		SceneNode* m_Parent;
		std::vector<SceneNode*> m_Children;
	};

	struct RootSceneNode
	{
		std::vector<SceneNode> m_Children;
	};

	void SceneObject_Sort(RootSceneNode& target);
	void SceneObject_Update(RootSceneNode& target);
}