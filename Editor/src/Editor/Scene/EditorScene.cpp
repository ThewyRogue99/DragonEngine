#include "depch.h"
#include "EditorScene.h"

#include "Engine/Debug/Instrumentor.h"

#include "Engine/Renderer/Renderer2D.h"

namespace Engine
{
	EditorScene::EditorScene(const WString& Name) : Scene(Name)
	{
		PrimaryCamera = new EditorCamera(60.f, 16 / 9, 0.1f, 1000.f);
	}

	void EditorScene::OnUpdate(float DeltaTime)
	{
		DE_PROFILE_FUNCTION();

		if (!bShouldBlockEvents)
			PrimaryCamera->Update(DeltaTime);

		if (bShouldSimulate)
			PhysicsWorld.Update(DeltaTime);
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
			PhysicsWorld.Begin(this);
	}

	void EditorScene::OnSceneEnd()
	{
		if (bShouldSimulate)
			PhysicsWorld.End();
	}

	void EditorScene::CopyEditorScene(EditorScene* SceneRef)
	{
		Copy(SceneRef);

		if (SceneRef->PrimaryCamera)
			delete SceneRef->PrimaryCamera;

		SceneRef->PrimaryCamera = PrimaryCamera;
		SceneRef->bShouldBlockEvents = bShouldBlockEvents;
	}
}