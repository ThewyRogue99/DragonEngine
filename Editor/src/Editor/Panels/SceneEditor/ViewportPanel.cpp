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
#include "Engine/Asset/TextureManager.h"

#include "Editor/Tools/EditorTool.h"
#include "Editor/Tools/ResourceTool.h"

#include "Editor/Scene/EditorSceneManager.h"

#include "IconsFontAwesome6.h"

#include <imgui_internal.h>

#pragma warning(disable : 4312)

namespace Engine
{
	ViewportPanel::ViewportPanel() : EditorPanel(ICON_FA_TV "  Viewport")
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

		float CursorPosY = ImGui::GetCursorPosY();

		uint32_t BufferID = m_FrameBuffer->GetColorAttachmentRendererID();
		ImGui::Image((void*)BufferID, ImGui::GetContentRegionAvail(), ImVec2{0.f, 1.f}, ImVec2{1.f, 0.f});

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM", ImGuiDragDropFlags_AcceptBeforeDelivery);

			// Highlight when dragged to viewport
			if (payload)
			{
				PanelDragPayload::ContentBrowserItem Item;
				Item.FromData((uint8_t*)payload->Data);

				if (Item.ItemType == AssetType::Scene)
				{
					if (payload->IsPreview())
					{
						ImGui::GetForegroundDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 255, 0, 255));
					}
					if(payload->IsDelivery())
					{
						bool ShouldAcceptScene = !EditorTool::IsPlaying() && !EditorTool::IsSimulating();

						if (ShouldAcceptScene)
						{
							Ref<Asset> asset = AssetManager::LoadAsset(Item.GetID());

							EditorScene* NewScene = EditorSceneManager::CreateEditorScene(asset->GetInfo().Name);

							SceneSerializer::Deserialize(NewScene, asset->Metadata);
						}
					}
				}
				else if (Item.ItemType == AssetType::Texture)
				{
					if (payload->IsDelivery())
					{
						if (HoveredEntity.IsValid() && HoveredEntity.HasComponent<SpriteRendererComponent>())
						{
							auto& src = HoveredEntity.GetComponent<SpriteRendererComponent>();
							src.TextureID = Item.GetID();
							src.Texture = TextureManager::LoadTexture(src.TextureID);
						}
					}
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

				ImGuizmo::MODE mode = ImGuizmo::LOCAL;
				float snap = 0.f;
				switch (GizmoType)
				{
				case ImGuizmo::OPERATION::TRANSLATE:
				{
					if (PositionSnap.Enabled)
						snap = PositionSnap.Value;
				} break;
				case ImGuizmo::OPERATION::ROTATE:
				{
					if (RotationSnap.Enabled)
						snap = RotationSnap.Value;
				} break;
				case ImGuizmo::OPERATION::SCALE:
				{
					if (ScaleSnap.Enabled)
						snap = ScaleSnap.Value;

					mode = ImGuizmo::WORLD;
				} break;
				default:
					break;
				}

				ImGuizmo::Manipulate(
					glm::value_ptr(cameraView),
					glm::value_ptr(cameraProjection),
					(ImGuizmo::OPERATION)GizmoType,
					mode,
					glm::value_ptr(transform),
					nullptr,
					&snap
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

		RenderToolbar(CursorPosY);
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

	static bool RoundedButton(const char* Label, const ImVec2& Size, float Rounding = 0.f, bool IsSelected = false, ImDrawFlags CornerFlags = 0)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		const ImGuiStyle& style = ImGui::GetStyle();
		const ImGuiID id = window->GetID(Label);
		const ImVec2 label_size = ImGui::CalcTextSize(Label, nullptr, true);
		
		ImVec2 max1(window->DC.CursorPos.x + Size.x, window->DC.CursorPos.y + Size.y);

		const ImRect bb(window->DC.CursorPos, max1);
		ImGui::ItemSize(bb);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		const ImVec2 max2(bb.Max.x, bb.Max.y - style.FramePadding.y);
		const ImRect button_bb(bb.Min, max2);

		bool hovered = false, held = false;
		bool pressed = ImGui::ButtonBehavior(button_bb, id, &hovered, &held);

		ImVec4 color(1.f, 1.f, 1.f, 1.f);

		if (held || IsSelected)
		{
			color = style.Colors[ImGuiCol_ButtonActive];
		}
		else if (hovered)
		{
			color = style.Colors[ImGuiCol_ButtonHovered];
		}
		else
		{
			color = style.Colors[ImGuiCol_Button];
		}

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(button_bb.Min, button_bb.Max, ImGui::GetColorU32(color), Rounding, CornerFlags);

		const ImVec2 text_pos(button_bb.Min.x, button_bb.Min.y - ((button_bb.Min.y - button_bb.Max.y) / 2.f) - (label_size.y / 2.f));
		ImGui::RenderTextClipped(text_pos, button_bb.Max, Label, nullptr, &label_size, ImVec2(0.5f, 0.5f));

		return pressed;
	}

	void ViewportPanel::RenderToolbar(float CursorPosY)
	{
		const ImVec2 buttonSize(32.f, 24.f);

		ImGui::SetCursorPos(ImVec2(5.f, CursorPosY + 5.f));

		// Render Gizmo Tools
		{
			const float ToolsWidth = (buttonSize.x * 3.f);

			bool selected1 = (GizmoType == ImGuizmo::OPERATION::TRANSLATE);
			if (RoundedButton(
				ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT,
				buttonSize,
				5.f,
				selected1,
				ImDrawFlags_RoundCornersLeft
			)) {
				GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			}
			ImGui::SameLine(0.f, 0.f);

			bool selected2 = (GizmoType == ImGuizmo::OPERATION::ROTATE);
			if (RoundedButton(
				ICON_FA_ROTATE,
				buttonSize,
				0.f,
				selected2,
				0
			)) {
				GizmoType = ImGuizmo::OPERATION::ROTATE;
			}
			ImGui::SameLine(0.f, 0.f);

			bool selected3 = (GizmoType == ImGuizmo::OPERATION::SCALE);
			if (RoundedButton(
				ICON_FA_UP_RIGHT_AND_DOWN_LEFT_FROM_CENTER,
				buttonSize,
				5.f,
				selected3,
				ImDrawFlags_RoundCornersRight
			)) {
				GizmoType = ImGuizmo::OPERATION::SCALE;
			}
		}

		// Render Grid Tools
		{
			const float ToolsWidth = ((buttonSize.x * 2.f) * 3.f) + (2.f * 10.f);

			ImGui::SameLine((GetSize().x - ToolsWidth) - 5.f);

			// Translate Grid
			{
				if (RoundedButton(
					ICON_FA_TABLE_CELLS,
					buttonSize,
					5.f,
					PositionSnap.Enabled,
					ImDrawFlags_RoundCornersLeft
				)) {
					PositionSnap.Enabled = !PositionSnap.Enabled;
				}
				ImGui::SameLine(0.f, 0.f);

				// Get PositionSnap string
				char str[20];

				// Convert the float to a string with 6 decimal places
				sprintf(str, "%.2f", PositionSnap.Value);

				if (RoundedButton(
					str,
					buttonSize,
					5.f,
					false,
					ImDrawFlags_RoundCornersRight
				)) {
					PositionSnap.ShowPopup = true;
					RotationSnap.ShowPopup = false;
					ScaleSnap.ShowPopup = false;
				}

				ImGui::SameLine(0.f, 10.f);

				if (PositionSnap.ShowPopup)
				{
					if (((ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseDown(ImGuiMouseButton_Right)) && !ImGui::IsItemHovered()) || ImGui::IsKeyDown(ImGuiKey_Escape))
					{
						PositionSnap.ShowPopup = false;
					}
					else
					{
						ImGui::OpenPopup("SetPositionSnap");
					}
				}

				if (ImGui::BeginPopup("SetPositionSnap"))
				{
					ImGui::BeginChild("##SetPositionSnap", ImVec2(250.f, 40.f));
					{
						ImGui::SetCursorPos(ImVec2(20.f, 10.f));

						ImGui::DragFloat("Snap Value", &PositionSnap.Value, 0.5f, 1.f, 1000.f, "%.2f");
					}
					ImGui::EndChild();

					ImGui::EndPopup();
				}
			}

			// Rotate Grid
			{
				if (RoundedButton(
					ICON_FA_COMPASS_DRAFTING,
					buttonSize,
					5.f,
					RotationSnap.Enabled,
					ImDrawFlags_RoundCornersLeft
				)) {
					RotationSnap.Enabled = !RotationSnap.Enabled;
				}
				ImGui::SameLine(0.f, 0.f);

				// Get RotationSnap string
				char str[20];

				// Convert the float to a string with 6 decimal places
				sprintf(str, "%.2f", RotationSnap.Value);

				if (RoundedButton(
					str,
					buttonSize,
					5.f,
					false,
					ImDrawFlags_RoundCornersRight
				)) {
					PositionSnap.ShowPopup = false;
					RotationSnap.ShowPopup = true;
					ScaleSnap.ShowPopup = false;
				}
				ImGui::SameLine(0.f, 10.f);

				if (RotationSnap.ShowPopup)
				{
					if (((ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseDown(ImGuiMouseButton_Right)) && !ImGui::IsItemHovered()) || ImGui::IsKeyDown(ImGuiKey_Escape))
					{
						RotationSnap.ShowPopup = false;
					}
					else
					{
						ImGui::OpenPopup("SetRotationSnap");
					}
				}

				if (ImGui::BeginPopup("SetRotationSnap"))
				{
					ImGui::BeginChild("##SetRotationSnap", ImVec2(250.f, 40.f));
					{
						ImGui::SetCursorPos(ImVec2(20.f, 10.f));
						
						ImGui::DragFloat("Snap Value", &RotationSnap.Value, 5.f, 0.f, 180.f, "%.2f");
					}
					ImGui::EndChild();

					ImGui::EndPopup();
				}
			}

			// Scale Grid
			{
				if (RoundedButton(
					ICON_FA_MAXIMIZE,
					buttonSize,
					5.f,
					ScaleSnap.Enabled,
					ImDrawFlags_RoundCornersLeft
				)) {
					ScaleSnap.Enabled = !ScaleSnap.Enabled;
				}
				ImGui::SameLine(0.f, 0.f);

				// Get ScaleSnap string
				char str[20];

				// Convert the float to a string with 6 decimal places
				sprintf(str, "%.2f", ScaleSnap.Value);

				if (RoundedButton(
					str,
					buttonSize,
					5.f,
					false,
					ImDrawFlags_RoundCornersRight
				)) {
					PositionSnap.ShowPopup = false;
					RotationSnap.ShowPopup = false;
					ScaleSnap.ShowPopup = true;
				}

				if (ScaleSnap.ShowPopup)
				{
					if (((ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseDown(ImGuiMouseButton_Right)) && !ImGui::IsItemHovered()) || ImGui::IsKeyDown(ImGuiKey_Escape))
					{
						ScaleSnap.ShowPopup = false;
					}
					else
					{
						ImGui::OpenPopup("SetScaleSnap");
					}
				}

				if (ImGui::BeginPopup("SetScaleSnap"))
				{
					ImGui::BeginChild("##SetScaleSnap", ImVec2(250.f, 40.f));
					{
						ImGui::SetCursorPos(ImVec2(20.f, 10.f));

						ImGui::DragFloat("Snap Value", &ScaleSnap.Value, 0.25f, 0.25f, 10.f, "%.2f");
					}
					ImGui::EndChild();

					ImGui::EndPopup();
				}
			}
		}
	}

	bool ViewportPanel::OnMouseButtonPressedEvent(MouseButtonPressedEvent& event)
	{
		switch(event.GetMouseButton())
		{
			case MouseButtonInput::MouseButton_Left:
			{
				if (bIsViewportHovered && !ImGuizmo::IsUsing() && HoveredEntity != SelectedEntity && !Input::IsKeyPressed(KeyInput::Key_LeftAlt))
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

	void ViewportPanel::OnData(const WString& Name, void* Data, size_t size)
	{
		if (Name == L"SelectedEntity")
		{
			SelectedEntity = *(Entity*)Data;
		}
	}
}