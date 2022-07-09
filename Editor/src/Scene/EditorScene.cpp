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

		switch (CurrentSceneState)
		{
			case SceneState::Edit:
			{
				editorCamera.OnUpdate(DeltaTime);

				Renderer2D::BeginScene(editorCamera);

				auto view = SceneRegistry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
				for (auto entity : view)
				{
					auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);

					Renderer2D::DrawQuadSprite(transform.GetTransformMat4(), sprite, (int)entity);
				}

				Renderer2D::EndScene();
			} break;
			case Engine::Scene::SceneState::Play:
			{
				Scene::OnUpdate(DeltaTime);
			} break;
		}
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
}