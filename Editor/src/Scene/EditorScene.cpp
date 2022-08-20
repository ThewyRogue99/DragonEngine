#include "depch.h"
#include "EditorScene.h"

#include "Engine/Renderer/Renderer2D.h"

namespace Engine
{
	EditorScene::EditorScene(const CString& Name) : Scene(Name)
	{
		editorCamera = new EditorCamera(60.f, 16 / 9, 0.1f, 1000.f);
	}

	EditorScene::~EditorScene()
	{

	}

	void EditorScene::OnUpdate(float DeltaTime)
	{
		DE_PROFILE_FUNCTION();

		editorCamera->OnUpdate(DeltaTime);

		PrimaryCamera.Transform = editorCamera->GetTransform();

		if (bShouldSimulate)
			OnPhysics2DUpdate(DeltaTime);

		Render(DeltaTime);
	}

	void EditorScene::OnEvent(Event& event)
	{
		if(!bShouldBlockEvents)
			editorCamera->OnEvent(event);
	}

	void EditorScene::OnViewportResize(uint32_t width, uint32_t height)
	{
		Scene::OnViewportResize(width, height);

		editorCamera->SetViewportSize(width, height);
	}

	void EditorScene::OnSceneBegin()
	{
		PrimaryCamera.CameraPtr = editorCamera;
		PrimaryCamera.EntityHandle = Entity();
		PrimaryCamera.Transform = editorCamera->GetTransform();

		if (bShouldSimulate)
			OnPhysics2DStart();
	}

	void EditorScene::OnSceneEnd()
	{
		if (bShouldSimulate)
			OnPhysics2DEnd();
	}

	EditorScene* EditorScene::CopyEditorScene()
	{
		EditorScene* CopyScene = new EditorScene();

		CopyToRef(CopyScene);

		if (CopyScene->editorCamera)
			delete CopyScene->editorCamera;

		CopyScene->editorCamera = editorCamera;
		CopyScene->bShouldBlockEvents = bShouldBlockEvents;

		return CopyScene;
	}
}