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
		bool OnKeyPressedEvent(KeyPressedEvent& event);
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& event);

	private:
		Ref<Framebuffer> m_FrameBuffer;

		Ref<Scene> ActiveScene;

		glm::vec2 ViewportSize = { 0.f, 0.f };

		bool bIsViewportFocused = false;
		bool bIsViewportHovered = false;

		glm::vec2 ViewportBounds[2];

		EditorCamera editorCamera;

		Entity HoveredEntity;

		int GizmoType = -1;

		// Panels
		SceneHierarchyPanel HPanel;
	};
}
