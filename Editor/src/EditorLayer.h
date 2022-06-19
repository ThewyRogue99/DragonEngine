#pragma once

#include <imgui/imgui.h>
#include <Engine/Core/ApplicationCore.h>
#include <Engine/Renderer/OrthographicCameraController.h>

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

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

		void NewScene();

		void OpenScene();
		void OpenScene(const wchar_t* path);

		void SaveScene();
		void SaveScene(const wchar_t* path);

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

		ContentBrowserPanel CBPanel;
	};
}
