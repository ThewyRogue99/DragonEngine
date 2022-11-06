#include "depch.h"
#include "EditorScene.h"

#include "Engine/Renderer/Renderer2D.h"

namespace Engine
{
	EditorScene::EditorScene(const CString& Name) : Scene(Name)
	{
		PrimaryCamera = new EditorCamera(60.f, 16 / 9, 0.1f, 1000.f);
	}

	EditorScene::~EditorScene()
	{

	}

	void EditorScene::OnUpdate(float DeltaTime)
	{
		DE_PROFILE_FUNCTION();

		PrimaryCamera->Update(DeltaTime);

		if (bShouldSimulate)
			OnPhysics2DUpdate(DeltaTime);

		Render(DeltaTime);
	}

	void EditorScene::OnEvent(Event& event)
	{
		if(!bShouldBlockEvents)
			((EditorCamera*)PrimaryCamera)->OnEvent(event);
	}

	void EditorScene::OnViewportResize(uint32_t width, uint32_t height)
	{
		Scene::OnViewportResize(width, height);

		((EditorCamera*)PrimaryCamera)->SetViewportSize((float)width, (float)height);
	}

	void EditorScene::OnSceneBegin()
	{
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

		if (CopyScene->PrimaryCamera)
			delete CopyScene->PrimaryCamera;

		CopyScene->PrimaryCamera = PrimaryCamera;
		CopyScene->bShouldBlockEvents = bShouldBlockEvents;

		return CopyScene;
	}
}