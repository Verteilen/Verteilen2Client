#include "engine.h"
#include <map>
#include <system/config.h>
#include <fstream>
#include <filesystem>

void StartCounting(Aktion::Engine::Engine& engine) 
{
	if (engine.m_Time.m_Frame == 0)
		engine.m_Time.m_StartPoint = engine.m_Time.m_Start.now();
	engine.m_Time.m_DeltaPoint = engine.m_Time.m_Start.now();
}
void EndCounting(Aktion::Engine::Engine& engine) 
{
	engine.m_Time.m_Frame++;
	engine.m_Time.m_Time = std::chrono::duration_cast<std::chrono::duration<double>>(
		engine.m_Time.m_Start.now() - engine.m_Time.m_StartPoint).count();
	engine.m_Time.m_Delta = std::chrono::duration_cast<std::chrono::duration<double>>(
		engine.m_Time.m_Start.now() - engine.m_Time.m_DeltaPoint).count();
}

Aktion::Engine::EngineInitErrorCode Aktion::Engine::Engine_Init(Engine& engine)
{
	AKTION_LOG_INFO("Engine initialization start...");
	using namespace std::filesystem;
	path CurrentPath = current_path();
	path ProjectFolder = CurrentPath / "temp";
	bool useCustomPath = false;
	EngineInitErrorCode result = EngineInitErrorCode::None;

	if (engine.m_Parameters.find("--path") != engine.m_Parameters.end()) 
	{
		path p = path(engine.m_Parameters["--path"]);

		if (exists(p)) {
			ProjectFolder = path(p);
			useCustomPath = true;
		}
		else {
			AKTION_LOG_WARN("The argument --path of path cannot be found: %s", p.string().c_str());
			result = EngineInitErrorCode::PathNotFound;
			useCustomPath = false;
		}
	}

	if(!useCustomPath)
	{
		if (exists(ProjectFolder))
		{
			AKTION_LOG_WARN("Use temp project, and discover that temp folder already exist..., It will delete the entire temp and create a new one");
			remove_all(ProjectFolder);
		}
		create_directory(ProjectFolder);
	}

	path Asset = ProjectFolder / "Asset";
	path Blueprint = ProjectFolder / "Blueprint";
	path Model = ProjectFolder / "Model";
	path Video = ProjectFolder / "Video";
	path Image = ProjectFolder / "Image";
	path Audio = ProjectFolder / "Audio";
	create_directory(Asset);
	AKTION_LOG_DEBUG("Create folder: %s", Asset.string().c_str());
	create_directory(Blueprint);
	AKTION_LOG_DEBUG("Create folder: %s", Blueprint.string().c_str());
	create_directory(Model);
	AKTION_LOG_DEBUG("Create folder: %s", Model.string().c_str());
	create_directory(Video);
	AKTION_LOG_DEBUG("Create folder: %s", Video.string().c_str());
	create_directory(Image);
	AKTION_LOG_DEBUG("Create folder: %s", Image.string().c_str());
	create_directory(Audio);
	AKTION_LOG_DEBUG("Create folder: %s", Audio.string().c_str());
	
	Engine_SaveProjectFile(engine);

	engine.m_ProjectInfo.m_AbsolutePath = ProjectFolder;

	AKTION_LOG_INFO("Use project path: %s", ProjectFolder.string().c_str());
	AKTION_LOG_INFO("Engine initialization end");
	return result;
}
void Aktion::Engine::Engine_MainLoop(Engine& engine)
{	
	StartCounting(engine);
	SceneObject_Update(engine.m_Root);
	EndCounting(engine);
}
void Aktion::Engine::Engine_Reset(Engine& engine)
{
	engine.m_Time.m_Frame = 0;
	engine.m_Time.m_Delta = 0;
	engine.m_Time.m_Time = 0;
}

void Aktion::Engine::Engine_SaveProjectFile(Engine& engine)
{
	using namespace std::filesystem;
	path CurrentPath = current_path();
	path Temp = CurrentPath / "temp";

	std::ofstream outf;
	outf.open(Temp / SETTING_FILE, std::ios::binary | std::ios::out);
	outf.write((char*)&engine.m_ProjectInfo.m_Setting, sizeof(ProjectSetting));
	outf.flush();
	outf.close();
}

void Aktion::Engine::Engine_LoadProjectFile(Engine& engine)
{
	using namespace std::filesystem;
	path CurrentPath = current_path();
	path Temp = CurrentPath / "temp";

	std::ifstream inf;
	inf.open(Temp / SETTING_FILE, std::ios::binary | std::ios::in);
	inf.read((char*)&engine.m_ProjectInfo.m_Setting, sizeof(ProjectSetting));
	inf.close();
}
