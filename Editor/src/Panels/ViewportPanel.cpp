#include "depch.h"
#include "ViewportPanel.h"

#include "Engine/Renderer/RenderCommand.h"

#include <glm/glm.hpp>
#include "Engine/Math/Math.h"

#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"

namespace Engine
{
	ViewportPanel::ViewportPanel() : EditorPanel("Viewport")
	{
		
	}

	void ViewportPanel::OnCreate()
	{
		EditorPanel::OnCreate();

		FramebufferSpecification Specs;

		Specs.Attachments = {
			FramebufferTextureFormat::RGBA8,
			FramebufferTextureFormat::RED_INTEGER,
			FramebufferTextureFormat::Depth
		};

		Specs.Width = GetWidth();
		Specs.Height = GetHeight();

		m_FrameBuffer = Framebuffer::Create(Specs);

		SetPanelStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.f, 0.f });
	}

	void ViewportPanel::OnUpdate(float DeltaTime)
	{
		auto [MouseX, MouseY] = GetPanelRelativeMousePos();
		ImVec2 ViewportSize = GetSize();

		if (MouseX >= 0 && MouseY >= 0 && MouseX < (int)ViewportSize.x && MouseY < (int)ViewportSize.y)
		{
			int pixelData = m_FrameBuffer->ReadPixel(1, MouseX, MouseY);

			if (pixelData == -1)
				HoveredEntity = { };
			else
				HoveredEntity = { (entt::entity)pixelData, ActiveScene };
		}
	}

	void ViewportPanel::OnRender(float DeltaTime)
	{
		EditorPanel::OnRender(DeltaTime);

		m_FrameBuffer->Bind();

		RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		RenderCommand::Clear();

		m_FrameBuffer->ClearAttachment(1, -1);

		ActiveScene->OnUpdate(DeltaTime);

		uint32_t BufferID = m_FrameBuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)BufferID, GetSize(), ImVec2{0.f, 1.f}, ImVec2{1.f, 0.f});

		if (ImGui::BeginDragDropTarget())
		{
			// Highlight when dragged to viewport
			ImGui::GetForegroundDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 0, 255));

			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;

				//OpenScene(path);
			}

			ImGui::EndDragDropTarget();
		}

		//Gizmos
		Entity* selectedEntity = (Entity*)GetData(TEXT("SelectedEntity"));

		if (selectedEntity && selectedEntity->IsValid())
		{
			ImGuizmo::SetOrthographic(false);

			ImGuizmo::SetDrawlist();

			auto firstBound = GetPanelBound(0);
			auto secondBound = GetPanelBound(1);

			ImGuizmo::SetRect(
				firstBound.x,
				firstBound.y,
				secondBound.x - firstBound.x,
				secondBound.y - firstBound.y
			);

			EditorCamera editorCamera = ActiveScene->GetEditorCamera();

			const glm::mat4& cameraProjection = editorCamera.GetProjection();
			glm::mat4 cameraView = editorCamera.GetViewMatrix();

			auto& tc = selectedEntity->GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransformMat4();

			if (GizmoType < 0)
				GizmoType = ImGuizmo::OPERATION::TRANSLATE;

			ImGuizmo::Manipulate(
				glm::value_ptr(cameraView),
				glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)GizmoType,
				GizmoType == ImGuizmo::OPERATION::SCALE ? ImGuizmo::LOCAL : ImGuizmo::WORLD,
				glm::value_ptr(transform)
			);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 position, rotation, scale;
				Math::DecomposeTransform(transform, position, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Position = position;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}

		if (ImGui::IsWindowFocused())
			bIsViewportFocused = true;
		else
			bIsViewportFocused = false;

		if (ImGui::IsWindowHovered())
			bIsViewportHovered = true;
		else
			bIsViewportHovered = false;

		ActiveScene->SetShouldBlockEvents(!bIsViewportHovered);

		m_FrameBuffer->Unbind();
	}

	void ViewportPanel::OnResize(uint32_t width, uint32_t height)
	{
		m_FrameBuffer->Resize(width, height);

		ActiveScene->OnViewportResize(width, height);
	}

	void ViewportPanel::OnEvent(Event& event)
	{
		EventDispatcher d(event);
		d.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonPressedEvent));
		d.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnKeyPressedEvent));

		ActiveScene->OnEvent(event);
	}

	void ViewportPanel::OnData(const CString& Name, void* Data, size_t size)
	{
		if (Name == TEXT("Scene"))
		{
			memcpy(&ActiveScene, Data, size);

			ImVec2 size = GetSize();
			ActiveScene->OnViewportResize((uint32_t)size.x, (uint32_t)size.y);
		}
	}

	bool ViewportPanel::OnMouseButtonPressedEvent(MouseButtonPressedEvent& event)
	{
		switch(event.GetMouseButton())
		{
			case MouseButtonInput::MouseButton_Left:
			{
				if (bIsViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(KeyInput::Key_LeftAlt))
					AddData(TEXT("SelectedEntity"), &HoveredEntity, sizeof(HoveredEntity));

				return true;
			}
			default:
				return false;
		}
	}

	bool ViewportPanel::OnKeyPressedEvent(KeyPressedEvent& event)
	{
		switch (event.GetKey())
		{
			case KeyInput::Key_Q:
			{
				if(bIsViewportFocused)
					GizmoType = ImGuizmo::OPERATION::TRANSLATE;

				return true;
			}
			case KeyInput::Key_W:
			{
				if (bIsViewportFocused)
					GizmoType = ImGuizmo::OPERATION::ROTATE;

				return true;
			}
			case KeyInput::Key_E:
			{
				if (bIsViewportFocused)
					GizmoType = ImGuizmo::OPERATION::SCALE;

				return true;
			}
			default:
				return false;
		}
	}
}