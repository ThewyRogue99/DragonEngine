#include "depch.h"
#include "SceneManager.h"

#include "Engine/Core/Log.h"

#include <algorithm>

namespace Engine
{
	SceneManager::CallbackManager SceneManager::OnSetActiveSceneManager = SceneManager::CallbackManager();

	std::vector<SceneManager::SceneData> SceneManager::SceneList = { };

	Ref<Scene> SceneManager::ActiveScene = nullptr;

	bool SceneManager::AddScene(const CString& Tag, Ref<Scene> SceneRef, bool Replace)
	{
		Ref<Scene> cpy = GetScene(Tag);

		if (!cpy)
		{
			SceneList.push_back({ Tag, SceneRef });

			return true;
		}
		else if (Replace)
		{
			return ReplaceScene(Tag, SceneRef);
		}

		DE_CORE_WARN("Scene with the given tag already exists");
		return false;
	}

	Ref<Scene> SceneManager::GetActiveScene()
	{
		return ActiveScene;
	}

	Ref<Scene> SceneManager::GetScene(const CString& Tag)
	{
		auto it = std::find_if(SceneList.begin(), SceneList.end(), [Tag](SceneData& Data)
		{
			return Data.Tag == Tag;
		});

		if (it != SceneList.end())
			return (*it).SceneRef;

		return nullptr;
	}

	bool SceneManager::ReplaceScene(const CString& Tag, Ref<Scene> SceneRef)
	{
		auto it = std::find_if(SceneList.begin(), SceneList.end(), [Tag](SceneData& Data)
		{
			return Data.Tag == Tag;
		});

		if (it != SceneList.end())
		{
			bool isActive = IsActiveScene((*it).Tag);

			Ref<Scene> copyRef = (*it).SceneRef;
			(*it).SceneRef = SceneRef;

			if (isActive)
				SetActiveScene((*it).Tag);

			return true;
		}

		DE_CORE_WARN("Scene with the given tag doesn't exist");
		return false;
	}

	bool SceneManager::RemoveScene(const CString& Tag)
	{
		auto it = std::remove_if(SceneList.begin(), SceneList.end(), [Tag](SceneData& Data)
		{
			return Data.Tag == Tag;
		});

		if (it != SceneList.end())
		{
			if (IsActiveScene((*it).Tag))
				RemoveActiveScene();

			SceneList.erase(it);
			return true;
		}

		DE_CORE_WARN("Scene with the given tag doesn't exist");
		return false;
	}

	bool SceneManager::SetActiveScene(const CString& Tag)
	{
		Ref<Scene> NewActiveScene = GetScene(Tag);

		if (NewActiveScene)
		{
			ActiveScene = NewActiveScene;

			OnSetActiveSceneManager.Run(ActiveScene);

			return true;
		}

		DE_CORE_WARN("Scene with the given tag doesn't exist");
		return false;
	}

	bool SceneManager::IsActiveScene(const CString& Tag)
	{
		Ref<Scene> ref = GetScene(Tag);

		return ActiveScene == ref;
	}

	void SceneManager::RemoveActiveScene()
	{
		if (ActiveScene)
		{
			ActiveScene = nullptr;

			OnSetActiveSceneManager.Run(ActiveScene);
		}
	}

	void SceneManager::CallbackManager::AddCallback(const CallbackType& callback)
	{
		CallbackList.push_back(callback);
	}

	void SceneManager::CallbackManager::Run(Ref<Scene> NewScene)
	{
		for (auto& callback : CallbackList)
			callback(NewScene);
	}
}