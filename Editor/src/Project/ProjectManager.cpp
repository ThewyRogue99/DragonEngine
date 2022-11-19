#include "ProjectManager.h"

#include "Engine/Asset/AssetManager.h"

#include <filesystem>
#include <fstream>

namespace Engine
{
	using DispatcherType = CallbackDispatcher<const Project&>;

	static Project ProjectData;
	static bool bIsProjectLoaded = false;

	static DispatcherType Dispatcher;

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

			Path /= ProjectName;
			std::filesystem::create_directory(Path);

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
			std::filesystem::create_directory(Path / TEXT("Assets"));

			std::filesystem::create_directory(Path / TEXT("Sources"));

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

				bIsProjectLoaded = true;
				SetAssetManager(Path.parent_path());

				Dispatcher.Run(ProjectData);

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