#include "EditorSceneManager.h"

namespace Engine
{
	static CallbackDispatcher<void, EditorScene*> OnEditorSceneChangeDispatch;
	static EditorScene* CurrentEditorScene = nullptr;

    EditorScene* EditorSceneManager::CreateEditorScene(const WString& Tag, bool DestroyCurrent)
    {
		EditorScene* NewScene = new EditorScene(Tag);

		SetEditorScene(NewScene, DestroyCurrent);

		return NewScene;
    }

	void EditorSceneManager::SetEditorScene(EditorScene* SceneRef, bool DestroyCurrent)
	{
		OnEditorSceneChangeDispatch.Run(SceneRef);

		if (CurrentEditorScene)
		{
			CString DebugName = TypeUtils::FromUTF16(CurrentEditorScene->GetName());
			DE_WARN(EditorSceneManager, "Removing current Editor Scene: {0}", DebugName.c_str());

			if (DestroyCurrent)
				delete CurrentEditorScene;

			CurrentEditorScene = nullptr;
		}

		CurrentEditorScene = SceneRef;
	}

	EditorScene* EditorSceneManager::GetEditorScene()
	{
		return CurrentEditorScene;
	}

	CallbackDispatcher<void, EditorScene*>::CallbackHandle EditorSceneManager::OnEditorSceneChange()
	{
		return OnEditorSceneChangeDispatch.GetHandle();
	}
}
