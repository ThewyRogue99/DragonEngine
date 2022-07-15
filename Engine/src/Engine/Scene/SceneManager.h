#pragma once

#include "Scene.h"

#include <vector>

namespace Engine
{
	class SceneManager
	{
	public:
		static bool AddScene(const CString& Tag, Ref<Scene> SceneRef, bool Replace = false);

		static Ref<Scene> GetActiveScene();

		static Ref<Scene> GetScene(const CString& Tag);

		static bool ReplaceScene(const CString& Tag, Ref<Scene> SceneRef);

		static bool RemoveScene(const CString& Tag);

		static bool SetActiveScene(const CString& Tag);

		static bool IsActiveScene(const CString& Tag);

		static void RemoveActiveScene();

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
		struct SceneData
		{
			CString Tag = CString();
			Ref<Scene> SceneRef = nullptr;
		};

		static CallbackManager OnSetActiveSceneManager;

		static std::vector<SceneData> SceneList;

		static Ref<Scene> ActiveScene;
	};
}
