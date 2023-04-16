#pragma once

#include "Scene.h"

#include "Engine/Core/CallbackDispatcher.h"

namespace Engine
{
	class ENGINE_API SceneManager
	{
	public:
		static Scene* CreateScene(const WString& Tag);

		static void AddScene(Scene* SceneRef);

		// Load a scene asset with its ID
		static Scene* LoadScene(const CString& SceneAssetID);

		// Load a scene asset with its Path and Name
		static Scene* LoadScene(const WString& Path, const WString& Name);

		static Scene* GetActiveScene();

		static Scene* GetScene(const WString& Tag);

		static bool ReplaceScene(const WString& Tag, Scene* SceneRef);

		static bool RemoveScene(const WString& Tag);

		static bool SetActiveScene(Scene* SceneRef);
		static bool SetActiveScene(const WString& Tag);

		static bool IsActiveScene(Scene* SceneRef);
		static bool IsActiveScene(const WString& Tag);

		static void Clear();

		static void ClearCallbacks();

		static CallbackDispatcher<void, Scene*>::CallbackHandle OnSetActiveScene();
	};
}