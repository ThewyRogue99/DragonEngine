#include "depch.h"
#include "EditorScene.h"

#include "Engine/Renderer/Renderer2D.h"

namespace Engine
{
	EditorScene::EditorScene(const CString& Name) : Scene(Name)
	{
		editorCamera = EditorCamera(60.f, 16 / 9, 0.1f, 1000.f);

		CurrentSceneState = SceneState::Edit;
	}

	void EditorScene::OnUpdate(float DeltaTime)
	{
		DE_PROFILE_FUNCTION();

		if(CurrentSceneState == SceneState::Edit)
		{
			editorCamera.OnUpdate(DeltaTime);

			Renderer2D::BeginScene(editorCamera, editorCamera.GetTransform());

			{
				auto view = SceneRegistry.view<TransformComponent, SpriteRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);

					Renderer2D::DrawQuadSprite(transform.GetTransformMat4(), sprite, (int)entity);
				}
			}

			{
				auto view = SceneRegistry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

					Renderer2D::DrawCircle(
						transform.GetTransformMat4(),
						circle.Color,
						circle.Thickness,
						circle.Fade,
						(int)entity
					);
				}
			}

			Renderer2D::EndScene();
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