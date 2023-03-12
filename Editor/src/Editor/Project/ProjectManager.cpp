#include "ProjectManager.h"

#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Editor/Scene/EditorSceneManager.h"

#include "ProjectTools.h"
#include "Engine/Scripting/ScriptEngine.h"
#include "Engine/Asset/AssetManager.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace Engine
{
	using DispatcherType = CallbackDispatcher<void, const Project&>;

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
		DE_LOG(
			ProjectManager, "Creating project '{0}' in '{1}' path",
			ProjectName.c_str(),
			TypeUtils::FromUTF16(ProjectPath).c_str()
		);

		std::filesystem::path Path = ProjectPath;

		if (std::filesystem::is_directory(Path) && std::filesystem::exists(Path))
		{
			// Create Directories
			std::filesystem::create_directories(Path / TEXT("Binary/Scripts"));

			std::filesystem::create_directory(Path / TEXT("Assets"));

			std::filesystem::create_directory(Path / TEXT("Source"));

			// Create metadata.dat file for AssetManager
			std::ofstream f(Path / TEXT("Assets/metadata.dat"), std::ios::out);
			f.close();

			SetAssetManager(Path);

			// Create Project File
			CString ProjectPath = Path / (ProjectName + ".deproject");

			std::ofstream pf(ProjectPath, std::ios::out | std::ios::binary);
			if (pf.is_open())
			{
				// Create a scene
				EditorScene* EScene = EditorSceneManager::CreateEditorScene(TEXT("Scene"));

				Ref<AssetMetadata> sceneData = AssetMetadata::Create();
				SceneSerializer::Serialize(EScene, sceneData);

				Ref<Asset> SceneAsset = AssetManager::CreateAsset(TEXT(""), TEXT("Scene"), AssetType::Scene, sceneData);

				// Set Project
				ProjectData.Name = ProjectName;
				ProjectData.Path = Path;
				ProjectData.StartSceneID = SceneAsset->GetInfo().ID;

				Ref<AssetMetadata> projectData = AssetMetadata::Create();
				ProjectData.Serialize(projectData);

				projectData->Write(pf);

				pf.close();

				bIsProjectLoaded = true;
				OnProjectLoad(ProjectData);
				Dispatcher.Run(ProjectData);


				DE_INFO(ProjectManager, "Successfully created project '{0}'", ProjectName.c_str());
				return true;
			}
		}

		DE_ERROR(ProjectManager, "Failed to create project '{0}'", ProjectName.c_str());
		return false;
	}

	bool ProjectManager::LoadProject(const CString& ProjectPath)
	{
		DE_LOG(
			ProjectManager, "Loading project in '{0}' path",
			TypeUtils::FromUTF16(ProjectPath).c_str()
		);

		std::filesystem::path Path = ProjectPath;

		if ((Path.extension() == ".deproject") && std::filesystem::exists(Path))
		{
			std::ifstream pf(Path, std::ios::in | std::ios::binary);
			if (pf.is_open())
			{
				Ref<AssetMetadata> data = AssetMetadata::Create();
				data->Read(pf);

				ProjectData.Deserialize(data);

				std::filesystem::path ParentPath = Path.parent_path();
				ProjectData.Path = ParentPath;

				SetAssetManager(ParentPath);
				OnProjectLoad(ProjectData);

				// Set Default Scene
				Ref<Asset> SceneAsset = AssetManager::LoadAsset(ProjectData.StartSceneID);

				if (SceneAsset->GetInfo().Type == AssetType::Scene)
				{
					EditorScene* EScene = EditorSceneManager::CreateEditorScene(SceneAsset->GetInfo().Name);
					SceneSerializer::Deserialize(EScene, SceneAsset->Metadata);
				}
				else
				{
					// Create a scene
					EditorScene* EScene = EditorSceneManager::CreateEditorScene(TEXT("Scene"));

					Ref<AssetMetadata> data;
					SceneSerializer::Serialize(EScene, data);

					if (AssetManager::CreateAsset(TEXT(""), TEXT("Scene"), AssetType::Scene, data))
					{
						Ref<Asset> NewSceneAsset = AssetManager::LoadAsset(TEXT(""), TEXT("Scene"));

						ProjectData.StartSceneID = NewSceneAsset->GetInfo().ID;
					}
					else return false;
				}

				bIsProjectLoaded = true;
				Dispatcher.Run(ProjectData);

				DE_INFO(ProjectManager, "Successfully loaded project '{0}'", ProjectData.Name.c_str());
				return true;
			}

		}

		DE_ERROR(ProjectManager, "Failed to load project in '{0}' path", TypeUtils::FromUTF16(ProjectPath).c_str());
		return false;
	}

	bool ProjectManager::CreateScript(const std::string& Name)
	{
		DE_LOG(ProjectManager, "Creating Script '{0}'", Name.c_str());

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

				DE_INFO("Successfully created Script '{0}'", Name.c_str());

				return true;
			}
		}

		DE_ERROR("Failed to create Script '{0}'", Name.c_str());
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

	void Project::Serialize(Ref<AssetMetadata> data)
	{
		MemoryMap& DataMap = data->GetFields();

		DataMap.SetStringField("Name", Name);
		DataMap.SetStringField("StartSceneID", StartSceneID);
	}

	void Project::Deserialize(Ref<AssetMetadata> data)
	{
		MemoryMap& DataMap = data->GetFields();

		Name = DataMap.GetStringField<char>("Name");
		StartSceneID = DataMap.GetStringField<char>("StartSceneID");
	}
}