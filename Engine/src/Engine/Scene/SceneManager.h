#pragma once

#include "Scene.h"

#include "Engine/Core/CallbackDispatcher.h"

namespace Engine
{
	class SceneManager
	{
	public:
		static bool AddScene(const CString& Tag, Scene* SceneRef, bool Replace = false);

		static Scene* GetActiveScene();

		static Scene* GetScene(const CString& Tag);

		static bool ReplaceScene(const CString& Tag, Scene* SceneRef);

		static bool RemoveScene(const CString& Tag);

		static bool SetActiveScene(const CString& Tag);

		static bool IsActiveScene(const CString& Tag);

		static void RemoveActiveScene();

		using DispatcherType = CallbackDispatcher<Scene*>;

		inline static DispatcherType::CallbackHandle OnSetActiveScene()
			{ return OnSetActiveSceneDispatch.GetHandle(); }

	private:
		struct SceneData
		{
			CString Tag = CString();
			Scene* SceneRef = nullptr;
		};

		static DispatcherType OnSetActiveSceneDispatch;

		static std::vector<SceneData> SceneList;

		static Scene* ActiveScene;
	};
}
