#pragma once

#include <Engine/Core/ApplicationCore.h>
#include <Engine/Renderer/OrthographicCameraController.h>

namespace Engine
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep DeltaTime) override;

		virtual void OnImGuiRender(Timestep DeltaTime) override;

		virtual void OnEvent(Event& event) override;

	private:
		Ref<Shader> m_Shader;
		Ref<VertexArray> m_VertexArray;

		Ref<Texture2D> m_SquareTexture;

		Ref<Framebuffer> m_FrameBuffer;

		OrthographicCameraController m_CameraController;

		glm::vec4 SquareColor = { 0.0f, 1.f, 0.f, 1.f };
		glm::vec2 SquarePosition = glm::vec3(0.f);
		glm::vec2 SquareSize = glm::vec3(1.f);

		float SquareRotationSpeed = 5.f;
	};
}
