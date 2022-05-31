#pragma once

#include <imgui/imgui.h>
#include <Engine/Core/ApplicationCore.h>
#include <Engine/Renderer/OrthographicCameraController.h>

#include "Panels/SceneHierarchyPanel.h"

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
		Ref<Texture2D> m_SquareTexture;

		Ref<Framebuffer> m_FrameBuffer;

		Ref<Scene> ActiveScene;

		OrthographicCameraController m_CameraController;

		glm::vec2 ViewportSize = { 0.f, 0.f };

		bool bUIShouldBlockEvents = false;

		// Panels
		SceneHierarchyPanel HPanel;
	};
}
