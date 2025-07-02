#pragma once
#include <string>

namespace Aktion::Engine
{
	/// <summary>
	/// Define the data block for a entity
	/// </summary>
	struct SceneComponent 
	{
		std::string m_Name;
		std::string m_Path;
	};
}