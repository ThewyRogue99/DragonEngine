#include "depch.h"
#include "SceneRenderer.h"

#include "Engine/Debug/Instrumentor.h"

#include "Framebuffer.h"
#include "UniformBuffer.h"
#include "Renderer2D.h"

#include <glm/glm.hpp>

namespace Engine
{
	static Scene* ActiveScene = nullptr;
	static Ref<UniformBuffer> CameraUniformBuffer = nullptr;

	void SceneRenderer::BeginScene(Scene* SceneRef, Ref<Framebuffer> FramebufferRef)
	{
		Camera* PrimaryCamera = SceneRef->PrimaryCamera;
		glm::mat4 ViewProjection = PrimaryCamera ? PrimaryCamera->GetProjection() * glm::inverse(PrimaryCamera->GetTransform()) : glm::mat4(0.f);

		if (!CameraUniformBuffer)
		{
			CameraUniformBuffer = UniformBuffer::Create(sizeof(ViewProjection), 0);
		}
		CameraUniformBuffer->SetData(&ViewProjection, sizeof(ViewProjection));

		Ref<RendererAPI> API = Renderer::GetRendererAPI();

		API->SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		API->Clear(FramebufferRef);

		FramebufferRef->ClearAttachment(1, -5);

		Renderer2D::BeginFrame(FramebufferRef);

		ActiveScene = SceneRef;
	}

	void SceneRenderer::EndScene()
	{
		ActiveScene = nullptr;
		Renderer2D::EndFrame();
	}

	void SceneRenderer::Render()
	{
		DE_PROFILE_FUNCTION();

		if (ActiveScene)
		{
			// Sprite Rendering
			{
				entt::basic_view<entt::entity, entt::get_t<Engine::TransformComponent, Engine::SpriteRendererComponent>, entt::exclude_t<>, void> view;

				{
					DE_PROFILE_SCOPE("Creating View");
					view = ActiveScene->SceneRegistry.view<TransformComponent, SpriteRendererComponent>();
				}

				for (auto entity : view)
				{
					QuadDrawProperties Props;

					{
						DE_PROFILE_SCOPE("Getting Components");

						auto& [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);

						Props.Transform = transform.GetTransformMat4();
						Props.Color = sprite.Color;
						Props.TextureRef = sprite.Texture;
						Props.TilingFactor = sprite.TilingFactor;
					}

					Renderer2D::DrawQuad(Props, (int)entity);
				}
			}

			// Circle Rendering
			{
				auto view = ActiveScene->SceneRegistry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

					CircleDrawProperties Props;
					Props.Transform = transform.GetTransformMat4();
					Props.Color = circle.Color;
					Props.Thickness = circle.Thickness;
					Props.Fade = circle.Fade;

					Renderer2D::DrawCircle(Props, (int)entity);
				}
			}
		}
	}
}