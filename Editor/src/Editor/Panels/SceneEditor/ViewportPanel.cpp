#include "depch.h"
#include "ViewportPanel.h"

#include "PanelDragPayload.h"

#include <glm/glm.hpp>
#include "Engine/Math/Math.h"

#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/SceneRenderer.h"

#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"

#include "Engine/Asset/Serializer/SceneSerializer.h"
#include "Engine/Asset/AssetManager.h"

#include "Editor/Tools/EditorTool.h"
#include "Editor/Scene/EditorSceneManager.h"

#pragma warning(disable : 4312)

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

		SetPanelStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

		ActiveScene = EditorSceneManager::GetEditorScene();

		EditorSceneManager::OnEditorSceneChange().AddCallback(BIND_CLASS_FN(ViewportPanel::OnSetActiveScene));
		EditorTool::OnBeginPlay().AddCallback(BIND_CLASS_FN(ViewportPanel::OnBeginPlay));
		EditorTool::OnEndPlay().AddCallback(BIND_CLASS_FN(ViewportPanel::OnEndPlay));
	}

	void ViewportPanel::OnUpdate(float DeltaTime)
	{
		auto [MouseX, MouseY] = GetPanelRelativeMousePos();
		ImVec2 ViewportSize = GetSize();

		if (MouseX >= 0 && MouseY >= 0 && MouseX < (int)ViewportSize.x && MouseY < (int)ViewportSize.y)
		{
			int pixelData = m_FrameBuffer->ReadPixel(1, (int)MouseX, (int)MouseY);

			if (pixelData == -1)
				HoveredEntity = { };
			else
				HoveredEntity = { (entt::entity)pixelData, ActiveScene };
		}

		ActiveScene->OnUpdate(DeltaTime);
	}

	void ViewportPanel::OnRender(float DeltaTime)
	{
		EditorPanel::OnRender(DeltaTime);

		SceneRenderer::BeginScene(ActiveScene, m_FrameBuffer);

		SceneRenderer::Render();

		uint32_t BufferID = m_FrameBuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)BufferID, GetSize(), ImVec2{0.f, 1.f}, ImVec2{1.f, 0.f});

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM", ImGuiDragDropFlags_AcceptBeforeDelivery);

			// Highlight when dragged to viewport
			if(payload && payload->IsPreview())
				ImGui::GetForegroundDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 0, 255));

			if (payload && payload->IsDelivery())
			{
				PanelDragPayload::ContentBrowserItem Item;
				Item.FromData((uint8_t*)payload->Data);

				bool ShouldAcceptScene = !EditorTool::IsPlaying() && !EditorTool::IsSimulating();

				if (ShouldAcceptScene && Item.ItemType == AssetType::Scene)
				{
					Ref<Asset> asset = AssetManager::LoadAsset(Item.GetID());

					EditorScene* NewScene = EditorSceneManager::CreateEditorScene(asset->GetInfo().Name);

					SceneSerializer::Deserialize(NewScene, asset->Metadata);
				}

			}

			ImGui::EndDragDropTarget();
		}

		//Gizmos
		if (SelectedEntity.IsValid())
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

			Camera* primaryCamera = ActiveScene->GetPrimaryCamera();

			if (primaryCamera)
			{
				const glm::mat4& cameraProjection = primaryCamera->GetProjection();
				glm::mat4 cameraView = primaryCamera->GetViewMatrix();

				auto& tc = SelectedEntity.GetComponent<TransformComponent>();
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
					if (ImGui::IsKeyPressed(ImGuiKey_LeftAlt) && !bIsCopyingEntity)
					{
						bIsCopyingEntity = true;

						Entity newEntity = ActiveScene->CopyEntity(SelectedEntity);
						AddData(TEXT("SelectedEntity"), &newEntity, sizeof(newEntity));
					}
					else
					{
						if (ImGui::IsKeyReleased(ImGuiKey_LeftAlt))
							bIsCopyingEntity = false;
					}

					glm::vec3 position, rotation, scale;
					Math::DecomposeTransform(transform, position, rotation, scale);

					glm::vec3 deltaRotation = rotation - tc.Rotation;
					tc.Position = position;
					tc.Rotation += deltaRotation;
					tc.Scale = scale;
				}
			}
		}

		if (ImGui::IsWindowHovered())
		{
			bIsViewportHovered = true;
		}
		else
		{
			bIsViewportHovered = false;
		}

		if (ImGui::IsWindowFocused())
		{
			bIsViewportFocused = true;

			if (!EditorTool::IsPlaying())
			{
				if(bIsViewportHovered)
					((EditorScene*)ActiveScene)->SetShouldBlockEvents(false);
				else
					((EditorScene*)ActiveScene)->SetShouldBlockEvents(true);
			}
		}
		else
		{
			bIsViewportFocused = false;

			if (!EditorTool::IsPlaying())
				((EditorScene*)ActiveScene)->SetShouldBlockEvents(true);
		}

		SceneRenderer::EndScene();
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

	void ViewportPanel::OnSetActiveScene(Scene* NewScene)
	{
		ActiveScene = NewScene;

		ImVec2 size = GetSize();
		ActiveScene->OnViewportResize((uint32_t)size.x, (uint32_t)size.y);
	}

	void ViewportPanel::OnBeginPlay()
	{
		SceneManager::OnSetActiveScene().AddCallback(BIND_CLASS_FN(ViewportPanel::OnSetActiveScene));
		OnSetActiveScene(SceneManager::GetActiveScene());
	}

	void ViewportPanel::OnEndPlay()
	{
		OnSetActiveScene(EditorSceneManager::GetEditorScene());
	}

	bool ViewportPanel::OnMouseButtonPressedEvent(MouseButtonPressedEvent& event)
	{
		switch(event.GetMouseButton())
		{
			case MouseButtonInput::MouseButton_Left:
			{
				if (bIsViewportHovered && !(ImGuizmo::IsOver() && HoveredEntity != SelectedEntity) && !Input::IsKeyPressed(KeyInput::Key_LeftAlt))
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

	void ViewportPanel::OnData(const CString& Name, void* Data, size_t size)
	{
		if (Name == L"SelectedEntity")
		{
			SelectedEntity = *(Entity*)Data;
		}
	}
}