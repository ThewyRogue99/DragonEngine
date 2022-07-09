#include "depch.h"
#include "SceneManager.h"

#include "Engine/Core/Log.h"

#include <algorithm>

namespace Engine
{
	Ref<Scene> SceneManager::CreateScene(const CString& Name)
	{
		Ref<Scene> cpy = GetSceneByName(Name);

		if (!cpy)
		{
			Ref<Scene> NewScene = CreateRef<Scene>(Name);

			SceneList.push_back(NewScene);

			return NewScene;
		}

		DE_CORE_WARN("Scene with the given name already exists");
		return nullptr;
	}

	bool SceneManager::AddScene(Ref<Scene> SceneRef)
	{
		Ref<Scene> cpy = GetSceneByName(SceneRef->GetName());

		if (!cpy)
		{
			SceneList.push_back(SceneRef);

			return true;
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

	void SceneManager::SetActiveScene(Ref<Scene> SceneRef)
	{
		Ref<Scene> cpy = GetSceneByName(SceneRef->GetName());

		if (cpy && (cpy == SceneRef))
		{
			ActiveScene = SceneRef;

			OnSetActiveSceneManager.Run(ActiveScene);
		}

		DE_CORE_WARN("Given Scene is not in SceneManager");
	}

	void SceneManager::SetActiveScene(const CString& Name)
	{
		Ref<Scene> NewActiveScene = GetSceneByName(Name);

		if (NewActiveScene)
		{
			ActiveScene = NewActiveScene;

			OnSetActiveSceneManager.Run(ActiveScene);
		}

		DE_CORE_WARN("Scene with the given name doesn't exist");
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