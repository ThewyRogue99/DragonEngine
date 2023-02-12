#pragma once

#include "Scene.h"

#include "Engine/Core/CallbackDispatcher.h"

namespace Engine
{
	class SceneManager
	{
	public:
		static Scene* CreateScene(const CString& Tag);

		static void AddScene(Scene* SceneRef);

		// Load a scene asset with its ID
		static Scene* LoadScene(const std::string& SceneAssetID);

		// Load a scene asset with its Path and Name
		static Scene* LoadScene(const CString& Path, const CString& Name);

		static Scene* GetActiveScene();

		static Scene* GetScene(const CString& Tag);

		static bool ReplaceScene(const CString& Tag, Scene* SceneRef);

		static bool RemoveScene(const CString& Tag);

		static bool SetActiveScene(Scene* SceneRef);
		static bool SetActiveScene(const CString& Tag);

		static bool IsActiveScene(Scene* SceneRef);
		static bool IsActiveScene(const CString& Tag);

		static void Clear();

		static void ClearCallbacks();

		static CallbackDispatcher<void, Scene*>::CallbackHandle OnSetActiveScene();
	};
}