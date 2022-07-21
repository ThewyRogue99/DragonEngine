#include "depch.h"
#include "EditorScene.h"

#include "Engine/Renderer/Renderer2D.h"

namespace Engine
{
	EditorScene::EditorScene(const CString& Name) : Scene(Name)
	{
		editorCamera = EditorCamera(60.f, 16 / 9, 0.1f, 1000.f);
	}

	void EditorScene::OnUpdate(float DeltaTime)
	{
		DE_PROFILE_FUNCTION();

		editorCamera.OnUpdate(DeltaTime);

		PrimaryCamera.Transform = editorCamera.GetTransform();

		if (bShouldSimulate)
			OnPhysics2DUpdate(DeltaTime);

		Render(DeltaTime);
	}

	void EditorScene::OnEvent(Event& event)
	{
		if(!bShouldBlockEvents)
			editorCamera.OnEvent(event);
	}

	void EditorScene::OnViewportResize(uint32_t width, uint32_t height)
	{
		Scene::OnViewportResize(width, height);

		editorCamera.SetViewportSize(width, height);
	}

	void EditorScene::OnSceneBegin()
	{
		PrimaryCamera.CameraPtr = &editorCamera;
		PrimaryCamera.EntityHandle = Entity();
		PrimaryCamera.Transform = editorCamera.GetTransform();

		if (bShouldSimulate)
			OnPhysics2DStart();
	}

	void EditorScene::OnSceneEnd()
	{
		if (bShouldSimulate)
			OnPhysics2DEnd();
	}

	Ref<EditorScene> EditorScene::CopyEditorScene()
	{
		Ref<EditorScene> CopyScene = CreateRef<EditorScene>();

		CopyToRef(std::static_pointer_cast<Scene>(CopyScene));

		CopyScene->editorCamera = editorCamera;
		CopyScene->bShouldBlockEvents = bShouldBlockEvents;

		return CopyScene;
	}
}