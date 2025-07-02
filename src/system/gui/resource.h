#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <engine/project.h>

namespace Aktion::GUI 
{
	typedef struct AktionGUI;

	struct FileTree 
	{
		bool m_Dirty;
		bool m_Done;
		std::string m_Display;
		std::filesystem::directory_entry m_Path;
		std::vector<std::filesystem::path> m_Files;
		std::vector<FileTree> m_Directories;
	};

	struct AktionResource
	{
		std::filesystem::path m_CurrentPath;
		FileTree m_Root;
	};

	void GUI_DrawResourceContent(AktionGUI& target);
	void GUI_DrawResourcesView(AktionGUI& target, Engine::ProjectInfo& info);
}