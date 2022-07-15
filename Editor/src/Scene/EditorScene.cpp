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

			auto view = SceneRegistry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : view)
			{
				auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawQuadSprite(transform.GetTransformMat4(), sprite, (int)entity);
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