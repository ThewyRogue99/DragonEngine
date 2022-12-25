#include "ProjectManager.h"

#include "ProjectTools.h"
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/Asset/AssetManager.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace Engine
{
	using DispatcherType = CallbackDispatcher<const Project&>;

	static Project ProjectData;
	static bool bIsProjectLoaded = false;

	static void OnProjectLoad(const Project& p)
	{
		ProjectTools::SetProjectPath(p.Path);

		if(ProjectTools::ScriptProjectExists())
			ProjectTools::CompileScriptProject();
	}

	static DispatcherType Dispatcher = { OnProjectLoad };

	static void SetAssetManager(const std::filesystem::path& ProjectPath)
	{
		AssetManager::Init(ProjectPath / TEXT("Assets"));
	}

	bool ProjectManager::CreateProject(const CString& ProjectPath, const std::string& ProjectName)
	{
		std::filesystem::path Path = ProjectPath;

		if (std::filesystem::is_directory(Path) && std::filesystem::exists(Path))
		{
			// Set Project
			ProjectData.Name = ProjectName;
			ProjectData.Path = Path;

			// Create Project File
			CString ProjectPath = Path / (ProjectName + ".deproject");

			std::ofstream pf(ProjectPath, std::ios::out | std::ios::binary);
			if (pf.is_open())
			{
				AssetMetadata data;
				ProjectData.Serialize(data);

				data.Write(pf);

				pf.close();
			}

			// Create Directories
			std::filesystem::create_directories(Path / TEXT("Binary/Scripts"));

			std::filesystem::create_directory(Path / TEXT("Assets"));

			std::filesystem::create_directory(Path / TEXT("Source"));

			bIsProjectLoaded = true;
			SetAssetManager(Path);

			Dispatcher.Run(ProjectData);

			return true;
		}

		return false;
	}

	bool ProjectManager::LoadProject(const CString& ProjectPath)
	{
		std::filesystem::path Path = ProjectPath;

		if ((Path.extension() == ".deproject") && std::filesystem::exists(Path))
		{
			std::ifstream pf(Path, std::ios::in | std::ios::binary);
			if (pf.is_open())
			{
				AssetMetadata data;
				data.Read(pf);

				ProjectData.Deserialize(data);

				std::filesystem::path ParentPath = Path.parent_path();
				ProjectData.Path = ParentPath;

				bIsProjectLoaded = true;
				SetAssetManager(ParentPath);

				Dispatcher.Run(ProjectData);

				return true;
			}

		}

		return false;
	}

	bool ProjectManager::CreateScript(const std::string& Name)
	{
		bool IncludesSpace = Name.find_first_of(' ') != std::string::npos;
		if (Name != "Script" && !IncludesSpace)
		{
			std::filesystem::path SourcePath = ProjectData.Path;
			SourcePath /= TEXT("Source");
			SourcePath /= Name + ".cs";

			std::ofstream f(SourcePath, std::ios::out);
			if (f.is_open())
			{
				f << ProjectTools::GetScriptTemplateString(Name);

				f.close();

				ProjectTools::GenerateScriptProject();

				ProjectTools::CompileScriptProject();

				return true;
			}
		}

		return false;
	}

	bool ProjectManager::IsProjectLoaded()
	{
		return bIsProjectLoaded;
	}

	const Project& ProjectManager::GetLoadedProject()
	{
		return ProjectData;
	}

	DispatcherType::CallbackHandle ProjectManager::OnLoadProject()
	{
		return Dispatcher.GetHandle();
	}

	void Project::Serialize(AssetMetadata& data)
	{
		data.SetStringField("Name", Name);
	}

	void Project::Deserialize(AssetMetadata& data)
	{
		Name = data.GetStringField<char>("Name");
	}
}