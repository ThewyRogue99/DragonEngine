#include "depch.h"
#include "SceneManager.h"

#include "Engine/Core/Log.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/Serializer/SceneSerializer.h"

#include <algorithm>

namespace Engine
{
	static CallbackDispatcher<Scene*> OnSetActiveSceneDispatch;
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
		DE_CORE_ERROR("Cannot create {0} scene.(A scene with same tag already exists)", DebugName.c_str());
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
			DE_CORE_ERROR("Cannot add {0} scene.(A scene with same tag already exists)", DebugName.c_str());
		}
	}

	static void RunSetActiveScene(Scene* SceneRef)
	{
		OnSetActiveSceneDispatch.Run(SceneRef);
	}

	Scene* SceneManager::LoadScene(const std::string& SceneAssetID)
	{
		Asset SceneAsset = AssetManager::LoadAsset(SceneAssetID);

		if (SceneAsset.GetAssetType() == AssetType::Scene)
		{
			Scene* NewScene = new Scene();

			SceneSerializer::Deserialize(NewScene, *(SceneAsset.GetData()));

			if (!GetScene(NewScene->GetName()))
			{
				SceneList.push_back(NewScene);

				AssetManager::CloseAsset(SceneAsset);

				return NewScene;
			}

			std::string DebugName = TypeUtils::FromUTF16(NewScene->GetName());
			DE_CORE_ERROR("Cannot load {0} scene.(A scene with same tag already exists)", DebugName.c_str());

			AssetManager::CloseAsset(SceneAsset);
			delete NewScene;

			return nullptr;
		}

		return nullptr;
	}

	Scene* SceneManager::LoadScene(const CString& Path, const CString& Name)
	{
		Asset SceneAsset = AssetManager::LoadAsset(Path, Name);

		if (SceneAsset.GetAssetType() == AssetType::Scene)
		{
			Scene* NewScene = new Scene();

			SceneSerializer::Deserialize(NewScene, *(SceneAsset.GetData()));

			if (!GetScene(NewScene->GetName()))
			{
				SceneList.push_back(NewScene);

				AssetManager::CloseAsset(SceneAsset);

				return NewScene;
			}

			std::string DebugName = TypeUtils::FromUTF16(NewScene->GetName());
			DE_CORE_ERROR("Cannot load {0} scene.(A scene with same tag already exists)", DebugName.c_str());

			AssetManager::CloseAsset(SceneAsset);
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

		DE_CORE_WARN("Scene with the given tag doesn't exist");
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

		DE_CORE_WARN("Scene with the given tag doesn't exist");
		return false;
	}

	bool SceneManager::SetActiveScene(Scene* SceneRef)
	{
		if (SceneRef)
		{
			if (ActiveScene != SceneRef)
			{
				ActiveScene = SceneRef;

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

		DE_CORE_WARN("Scene with the given tag doesn't exist");
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

	CallbackDispatcher<Scene*>::CallbackHandle SceneManager::OnSetActiveScene()
	{
		return OnSetActiveSceneDispatch.GetHandle();
	}
}