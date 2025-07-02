#include "resource.h"
#include <filesystem>
#include <system/config.h>
#include <imgui.h>
#include <Imgui/utility/splitter.h>
#include <system/gui.h>
#include <system/gui/panel.h>



void ReadFolderStruct(Aktion::GUI::FileTree& tree, Aktion::Engine::ProjectInfo& info)
{
	for (const auto& entry : std::filesystem::directory_iterator(tree.m_Path))
	{
		if (entry.is_directory()) 
		{
			std::string foldername = entry.path().filename().string();
			Aktion::GUI::FileTree newtree = Aktion::GUI::FileTree();
			newtree.m_Path = entry;
			newtree.m_Display = entry.path().filename().string();

			ReadFolderStruct(newtree, info);
			tree.m_Directories.push_back(newtree);
		}
		else if (entry.is_regular_file()) 
		{
			tree.m_Files.push_back(entry.path());
		}
		tree.m_Done = true;
	}
}

std::vector<std::string> ProcessFileWatcher(Aktion::GUI::AktionGUI& target, Aktion::Engine::ProjectInfo& info)
{
	std::vector<std::string> buffer = std::vector<std::string>();

	// First time reading
	if (!target.m_Resource.m_Root.m_Done) 
	{
		target.m_Resource.m_Root.m_Path = std::filesystem::directory_entry(info.m_AbsolutePath);
		target.m_Resource.m_Root.m_Display = "Root";
		target.m_Resource.m_CurrentPath = info.m_AbsolutePath;
		ReadFolderStruct(target.m_Resource.m_Root, info);
	}

	return buffer;
}

void RenderFolderTree(Aktion::GUI::AktionGUI& host, Aktion::GUI::FileTree& target)
{
	int flags = ImGuiTreeNodeFlags_SpanFullWidth;
	if (host.m_Resource.m_CurrentPath == target.m_Path.path())
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}
	bool open = ImGui::TreeNodeEx(target.m_Display.c_str(), flags);
	if (ImGui::IsItemClicked())
	{
		host.m_Resource.m_CurrentPath = target.m_Path.path();
	}
	if (open)
	{
		for (auto i = target.m_Directories.begin(); i < target.m_Directories.end(); i++) 
		{
			RenderFolderTree(host, (*i));
		}
		ImGui::TreePop();
	}
}

void DrawTreeFileView(Aktion::GUI::AktionGUI& target, Aktion::Engine::ProjectInfo& info)
{
	if (ImGui::BeginChild("Resources View Tree", ImVec2(target.m_Layout.m_ResourceLeft - 4.0f, 0)))
	{
		RenderFolderTree(target, target.m_Resource.m_Root);
	}
	ImGui::EndChild();
}

void DrawListFileView(Aktion::GUI::AktionGUI& target, Aktion::Engine::ProjectInfo& info)
{
	if (ImGui::BeginChild("Resources View List"))
	{
		for (const auto& entry : std::filesystem::directory_iterator(target.m_Resource.m_CurrentPath)) 
		{
			if (entry.is_regular_file()) {
				if (ImGui::Selectable(entry.path().filename().string().c_str())) {

				}
			}
		}
	}
	ImGui::EndChild();
}

void Aktion::GUI::GUI_DrawResourceContent(Aktion::GUI::AktionGUI& target)
{
	if (ImGui::BeginChild("Resources Content", ImVec2(0, target.m_Layout.m_ResourcePanelTop - 4.0f)));
	{
		ImGui::Text("View Res Content");
	}
	ImGui::EndChild();
}

void Aktion::GUI::GUI_DrawResourcesView(Aktion::GUI::AktionGUI& target, Engine::ProjectInfo& info)
{
	if(ImGui::BeginChild("Resources View", ImVec2(0, 0)));
	{
		Aktion::GUI::Splitter(true, 4.0f, &target.m_Layout.m_ResourceLeft, &target.m_Layout.m_ResourceRight, 150.0f, 250.0f);
		std::vector<std::string> changed = ProcessFileWatcher(target, info);
		DrawTreeFileView(target, info);
		ImGui::SameLine(0, 12.0f);
		DrawListFileView(target, info);
	}
	ImGui::EndChild();
}
