#include "depch.h"
#include "SceneManager.h"

#include "Engine/Core/Log.h"

#include <algorithm>

namespace Engine
{
	SceneManager::CallbackManager SceneManager::OnSetActiveSceneManager = SceneManager::CallbackManager();

	std::vector<Ref<Scene>> SceneManager::SceneList = { };

	Ref<Scene> SceneManager::ActiveScene = nullptr;

	Ref<Scene> SceneManager::CreateScene(const CString& Name, bool Replace)
	{
		Ref<Scene> cpy = GetSceneByName(Name);

		if (!cpy)
		{
			Ref<Scene> NewScene = CreateRef<Scene>(Name);

			SceneList.push_back(NewScene);

			return NewScene;
		}
		else if (Replace)
		{
			Ref<Scene> NewScene = CreateRef<Scene>(Name);

			ReplaceScene(Name, NewScene);

			return NewScene;
		}

		DE_CORE_WARN("Scene with the given name already exists");
		return nullptr;
	}

	bool SceneManager::AddScene(Ref<Scene> SceneRef, bool Replace)
	{
		Ref<Scene> cpy = GetSceneByName(SceneRef->GetName());

		if (!cpy)
		{
			SceneList.push_back(SceneRef);

			return true;
		}
		else if (Replace)
		{
			return ReplaceScene(SceneRef->GetName(), SceneRef);
		}

		DE_CORE_WARN("Scene with the given name already exists");
		return false;
	}

	Ref<Scene> SceneManager::GetActiveScene()
	{
		return ActiveScene;
	}

	Ref<Scene> SceneManager::GetSceneByName(const CString& Name)
	{
		auto it = std::find_if(SceneList.begin(), SceneList.end(), [Name](Ref<Scene>& scene)
		{
			return scene->GetName() == Name;
		});

		if (it != SceneList.end())
			return *it;

		return nullptr;
	}

	bool SceneManager::ReplaceScene(const CString& ReplaceName, Ref<Scene> SceneRef)
	{
		auto it = std::find_if(SceneList.begin(), SceneList.end(), [ReplaceName, SceneRef](Ref<Scene>& scene)
		{
			return scene->GetName() == ReplaceName;
		});

		if (it != SceneList.end())
		{
			Ref<Scene> copyRef = *it;
			*it = SceneRef;

			if (IsActiveScene(copyRef))
				SetActiveScene(*it);

			return true;
		}

		DE_CORE_WARN("Scene with the given name doesn't exist");
		return false;
	}

	bool SceneManager::SetActiveScene(Ref<Scene> SceneRef)
	{
		Ref<Scene> cpy = GetSceneByName(SceneRef->GetName());

		if (cpy && (cpy == SceneRef))
		{
			ActiveScene = SceneRef;

			OnSetActiveSceneManager.Run(ActiveScene);

			return true;
		}

		DE_CORE_WARN("Given Scene is not in SceneManager");
		return false;
	}

	bool SceneManager::SetActiveScene(const CString& Name)
	{
		Ref<Scene> NewActiveScene = GetSceneByName(Name);

		if (NewActiveScene)
		{
			ActiveScene = NewActiveScene;

			OnSetActiveSceneManager.Run(ActiveScene);

			return true;
		}

		DE_CORE_WARN("Scene with the given name doesn't exist");
		return false;
	}

	bool SceneManager::IsActiveScene(Ref<Scene> SceneRef)
	{
		return SceneRef == ActiveScene;
	}

	bool SceneManager::IsActiveScene(const CString& Name)
	{
		Ref<Scene> ref = GetSceneByName(Name);

		return ActiveScene == ref;
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