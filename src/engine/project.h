#pragma once
#include <string>
#include <filesystem>

namespace Aktion::Engine 
{
	struct ProjectSetting 
	{
		bool m_DefaultNoEditor;
	};

	struct ProjectInfo 
	{
		bool m_Dirty;
		std::filesystem::path m_AbsolutePath;
		ProjectSetting m_Setting;
	};
}