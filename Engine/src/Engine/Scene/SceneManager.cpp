#include "depch.h"
#include "SceneManager.h"

#include "Engine/Debug/Log.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"

#include <algorithm>

namespace Engine
{
	static CallbackDispatcher<void, Scene*> OnSetActiveSceneDispatch;
	static std::vector<Scene*> SceneList = { };
	static Scene* ActiveScene = nullptr;

	Scene* SceneManager::CreateScene(const CString& Tag)
	{
		Scene* cpy = GetScene(Tag);

		if (!cpy)
		{
			Scene* NewScene = new Scene(Tag);

			SceneList.push_back(NewScene);

			return NewScene;
		}

		std::string DebugName = TypeUtils::FromUTF16(cpy->GetName());
		DE_ERROR(SceneManager, "Cannot create scene '{0}'.(A scene with same tag already exists)", DebugName.c_str());
		return nullptr;
	}

	void SceneManager::AddScene(Scene* SceneRef)
	{
		Scene* cpy = GetScene(SceneRef->GetName());

		if (!cpy)
		{
			SceneList.push_back(SceneRef);
		}
		else
		{
			std::string DebugName = TypeUtils::FromUTF16(cpy->GetName());
			DE_ERROR(SceneManager, "Cannot add scene '{0}'.(A scene with same tag already exists)", DebugName.c_str());
		}
	}

	static void RunSetActiveScene(Scene* SceneRef)
	{
		OnSetActiveSceneDispatch.Run(SceneRef);
	}

	Scene* SceneManager::LoadScene(const std::string& SceneAssetID)
	{
		Ref<Asset> SceneAsset = AssetManager::LoadAsset(SceneAssetID);

		if (SceneAsset->GetInfo().Type == AssetType::Scene)
		{
			Scene* NewScene = new Scene();

			SceneSerializer::Deserialize(NewScene, SceneAsset->Metadata);

			if (!GetScene(NewScene->GetName()))
			{
				SceneList.push_back(NewScene);

				return NewScene;
			}

			std::string DebugName = TypeUtils::FromUTF16(NewScene->GetName());
			DE_ERROR(SceneManager, "Cannot load scene '{0}'.(A scene with same tag already exists)", DebugName.c_str());

			delete NewScene;

			return nullptr;
		}

		return nullptr;
	}

	Scene* SceneManager::LoadScene(const CString& Path, const CString& Name)
	{
		Ref<Asset> SceneAsset = AssetManager::LoadAsset(Path, Name);

		if (SceneAsset->GetInfo().Type == AssetType::Scene)
		{
			Scene* NewScene = new Scene();

			SceneSerializer::Deserialize(NewScene, SceneAsset->Metadata);

			if (!GetScene(NewScene->GetName()))
			{
				SceneList.push_back(NewScene);

				return NewScene;
			}

			std::string DebugName = TypeUtils::FromUTF16(NewScene->GetName());
			DE_ERROR(SceneManager, "Cannot load scene '{0}'.(A scene with same tag already exists)", DebugName.c_str());

			delete NewScene;

			return nullptr;
		}

		return nullptr;
	}

	Scene* SceneManager::GetActiveScene()
	{
		return ActiveScene;
	}

	Scene* SceneManager::GetScene(const CString& Tag)
	{
		auto it = std::find_if(SceneList.begin(), SceneList.end(), [Tag](Scene* Data)
		{
			return Data->GetName() == Tag;
		});

		if (it != SceneList.end())
			return *it;

		return nullptr;
	}

	bool SceneManager::ReplaceScene(const CString& Tag, Scene* SceneRef)
	{
		auto it = std::find_if(SceneList.begin(), SceneList.end(), [Tag](Scene* Data)
		{
			return Data->GetName() == Tag;
		});

		if (it != SceneList.end())
		{
			const CString& Tag = (*it)->GetName();

			bool isActive = IsActiveScene(Tag);

			Scene* copyRef = *it;

			(*it) = SceneRef;

			if (isActive)
				SetActiveScene(Tag);

			delete copyRef;

			return true;
		}

		DE_ERROR(SceneManager, "Scene with the tag '{0}' doesn't exist", TypeUtils::FromUTF16(Tag).c_str());
		return false;
	}

	bool SceneManager::RemoveScene(const CString& Tag)
	{
		auto it = std::remove_if(SceneList.begin(), SceneList.end(), [Tag](Scene* Data)
		{
			return Data->GetName() == Tag;
		});

		if (it != SceneList.end())
		{
			if (IsActiveScene(*it))
				SetActiveScene(nullptr);

			delete (*it);
			SceneList.erase(it);
			return true;
		}

		DE_ERROR(SceneManager, "Scene with the tag '{0}' doesn't exist", TypeUtils::FromUTF16(Tag).c_str());
		return false;
	}

	bool SceneManager::SetActiveScene(Scene* SceneRef)
	{
		if (SceneRef)
		{
			if (ActiveScene != SceneRef)
			{
				ActiveScene = SceneRef;

				DE_INFO(
					SceneManager, "Setting Active Scene to '{0}'",
					TypeUtils::FromUTF16(SceneRef->GetName()).c_str()
				);

				RunSetActiveScene(ActiveScene);
			}

			return true;
		}

		return false;
	}

	bool SceneManager::SetActiveScene(const CString& Tag)
	{
		Scene* NewActiveScene = GetScene(Tag);

		if (NewActiveScene)
			return SetActiveScene(NewActiveScene);

		DE_ERROR(SceneManager, "Scene with the tag '{0}' doesn't exist", TypeUtils::FromUTF16(Tag).c_str());
		return false;
	}

	bool SceneManager::IsActiveScene(Scene* SceneRef)
	{
		return ActiveScene == SceneRef;
	}

	bool SceneManager::IsActiveScene(const CString& Tag)
	{
		Scene* ref = GetScene(Tag);

		return IsActiveScene(ref);
	}

	void SceneManager::Clear()
	{
		DE_WARN(SceneManager, "Clearing SceneManager");

		ClearCallbacks();

		for (Scene* scene : SceneList)
			delete scene;

		ActiveScene = nullptr;
		SceneList.clear();
	}

	void SceneManager::ClearCallbacks()
	{
		OnSetActiveSceneDispatch.Clear();
	}

	CallbackDispatcher<void, Scene*>::CallbackHandle SceneManager::OnSetActiveScene()
	{
		return OnSetActiveSceneDispatch.GetHandle();
	}
}