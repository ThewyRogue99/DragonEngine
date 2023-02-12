#pragma once

#include "Engine/Scene/SceneManager.h"
#include "EditorScene.h"

#include "Engine/Core/CallbackDispatcher.h"

namespace Engine
{
	class EditorSceneManager
	{
	public:
		static EditorScene* CreateEditorScene(const CString& Tag, bool DestroyCurrent = true);

		static void SetEditorScene(EditorScene* SceneRef, bool DestroyCurrent = true);

		static EditorScene* GetEditorScene();

		static CallbackDispatcher<void, EditorScene*>::CallbackHandle OnEditorSceneChange();
	};
}