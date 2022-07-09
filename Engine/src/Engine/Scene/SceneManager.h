#pragma once

#include "Scene.h"

#include <vector>

namespace Engine
{
	class SceneManager
	{
	public:
		static Ref<Scene> CreateScene(const CString& Name);

		static bool AddScene(Ref<Scene> SceneRef);

		static Ref<Scene> GetActiveScene();

		static Ref<Scene> GetSceneByName(const CString& Name);

		static void SetActiveScene(Ref<Scene> SceneRef);
		static void SetActiveScene(const CString& Name);

		class CallbackManager;

		inline static const CallbackManager& OnSetActiveScene() { return OnSetActiveSceneManager; }

		class CallbackManager
		{
		public:
			using CallbackType = std::function<void(Ref<Scene>)>;

			void AddCallback(const CallbackType& callback);

			friend class SceneManager;

		private:
			void Run(Ref<Scene> NewScene);

		private:
			std::vector<CallbackType> CallbackList;
		};

	private:
		static CallbackManager OnSetActiveSceneManager;

		static std::vector<Ref<Scene>> SceneList;

		static Ref<Scene> ActiveScene;
	};
}
