#pragma once

#include "Scene.h"

#include <vector>

namespace Engine
{
	class SceneManager
	{
	public:
		static Ref<Scene> CreateScene(const CString& Name, bool Replace = false);

		static bool AddScene(Ref<Scene> SceneRef, bool Replace = false);

		static Ref<Scene> GetActiveScene();

		static Ref<Scene> GetSceneByName(const CString& Name);

		static bool ReplaceScene(const CString& ReplaceName, Ref<Scene> SceneRef);

		static bool SetActiveScene(Ref<Scene> SceneRef);
		static bool SetActiveScene(const CString& Name);

		static bool IsActiveScene(Ref<Scene> SceneRef);
		static bool IsActiveScene(const CString& Name);

		class CallbackManager
		{
		public:
			CallbackManager() = default;

			using CallbackType = std::function<void(Ref<Scene>)>;

			void AddCallback(const CallbackType& callback);

			friend class SceneManager;

		private:
			void Run(Ref<Scene> NewScene);

		private:
			std::vector<CallbackType> CallbackList = { };
		};

		inline static CallbackManager& OnSetActiveScene() { return OnSetActiveSceneManager; }

	private:
		static CallbackManager OnSetActiveSceneManager;

		static std::vector<Ref<Scene>> SceneList;

		static Ref<Scene> ActiveScene;
	};
}
