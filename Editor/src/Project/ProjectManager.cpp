#include "ProjectManager.h"

#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "../Scene/EditorSceneManager.h"

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
			// Create Directories
			std::filesystem::create_directories(Path / TEXT("Binary/Scripts"));

			std::filesystem::create_directory(Path / TEXT("Assets"));

			std::filesystem::create_directory(Path / TEXT("Source"));

			SetAssetManager(Path);

			// Create Project File
			CString ProjectPath = Path / (ProjectName + ".deproject");

			std::ofstream pf(ProjectPath, std::ios::out | std::ios::binary);
			if (pf.is_open())
			{
				// Create a scene
				EditorScene* EScene = EditorSceneManager::CreateEditorScene(TEXT("Scene"));

				AssetMetadata sceneData;
				SceneSerializer::Serialize(EScene, sceneData);

				AssetManager::CreateAsset(TEXT(""), TEXT("Scene"), sceneData, AssetType::Scene);
				Ref<Asset> SceneAsset = AssetManager::LoadAsset(TEXT(""), TEXT("Scene"));

				// Set Project
				ProjectData.Name = ProjectName;
				ProjectData.Path = Path;
				ProjectData.StartSceneID = SceneAsset->GetID();

				AssetMetadata projectData;
				ProjectData.Serialize(projectData);

				projectData.Write(pf);

				pf.close();

				bIsProjectLoaded = true;
				OnProjectLoad(ProjectData);
				Dispatcher.Run(ProjectData);

				return true;
			}

			return false;
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

				SetAssetManager(ParentPath);
				OnProjectLoad(ProjectData);

				// Set Default Scene
				Ref<Asset> SceneAsset = AssetManager::LoadAsset(ProjectData.StartSceneID);

				if (SceneAsset->GetAssetType() == AssetType::Scene)
				{
					EditorScene* EScene = EditorSceneManager::CreateEditorScene(SceneAsset->GetName());
					SceneSerializer::Deserialize(EScene, SceneAsset->GetData());
				}
				else
				{
					// Create a scene
					EditorScene* EScene = EditorSceneManager::CreateEditorScene(TEXT("Scene"));

					AssetMetadata data;
					SceneSerializer::Serialize(EScene, data);

					if (AssetManager::CreateAsset(TEXT(""), TEXT("Scene"), data, AssetType::Scene))
					{
						Ref<Asset> NewSceneAsset = AssetManager::LoadAsset(TEXT(""), TEXT("Scene"));

						ProjectData.StartSceneID = NewSceneAsset->GetID();
					}
					else return false;
				}

				bIsProjectLoaded = true;
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
		data.SetStringField("StartSceneID", StartSceneID);
	}

	void Project::Deserialize(AssetMetadata& data)
	{
		Name = data.GetStringField<char>("Name");
		StartSceneID = data.GetStringField<char>("StartSceneID");
	}
}