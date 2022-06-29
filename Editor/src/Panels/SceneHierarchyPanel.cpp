#include "depch.h"
#include "SceneHierarchyPanel.h"

#include "Engine/Scene/Components.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace Engine
{
	SceneHierarchyPanel::SceneHierarchyPanel() : EditorPanel("Scene Hierarchy")
	{

	}

	void SceneHierarchyPanel::OnRender(float DeltaTime)
	{
		EditorPanel::OnRender(DeltaTime);

		Context->SceneRegistry.each([&](auto entityID)
		{
			DrawEntityNode({ entityID, Context });
		});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			SetSelectedEntity({ });

		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				Context->CreateEntity(TEXT("Empty Entity"));
			}

			ImGui::EndPopup();
		}

		ImGui::Begin("Properties");

		if (SelectedEntity.IsValid())
			DrawComponents(SelectedEntity);

		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity, bool SetData)
	{
		if (entity.IsValid())
			SelectedEntity = entity;
		else
			SelectedEntity = { };

		if (SetData)
			AddData(TEXT("SelectedEntity"), &entity, sizeof(entity));
	}

	void SceneHierarchyPanel::OnData(const CString& Name, void* Data, size_t size)
	{
		EditorPanel::OnData(Name, Data, size);

		if (Name == TEXT("Scene"))
		{
			memcpy(&Context, Data, size);
		}
		else if(Name == TEXT("SelectedEntity"))
		{
			Entity entity = *static_cast<Entity*>(Data);

			if (entity.IsValid())
				SelectedEntity = entity;
			else
				SelectedEntity = { };
		}
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = (SelectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		std::string tag_str = TypeUtils::FromUTF16(tag);

		bool open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag_str.c_str());

		if (ImGui::IsItemClicked())
		{
			SetSelectedEntity(entity);
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				Context->DestroyEntity(entity);

				if (SelectedEntity == entity)
					SetSelectedEntity({ });
			}

			ImGui::EndPopup();
		}

		if (open)
		{
			ImGui::TreePop();
		}
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const char* name, Entity entity, UIFunction uiFunction, bool allowDelete = true)
	{
		static const ImGuiTreeNodeFlags flags =
			ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();

			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, name);

			bool removeComponent = false;
			if (allowDelete)
			{
				ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4.f, 4.f });

				float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y;

				ImGui::SameLine(contentRegionAvailable.x - lineHeight + 36.f);

				if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight }))
					ImGui::OpenPopup("ComponentSettings");

				if (ImGui::BeginPopup("ComponentSettings"))
				{
					if (ImGui::MenuItem("Remove Component"))
						removeComponent = true;

					ImGui::EndPopup();
				}

				ImGui::PopStyleVar();
			}

			if (open)
			{
				uiFunction(component);

				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	static void DrawVec3Control(const char* label, glm::vec3& values, float resetValue = 0.f, float columnWidth = 100.f)
	{
		auto& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label);

		ImGui::Columns(2);

		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.f, 0.f });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
		ImVec2 buttonSize = { lineHeight + 3.f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.f });

		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.f, 0.f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.f });

		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();

		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.f, 0.f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.f });

		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.f, 0.f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PopStyleColor(3);

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if(entity.HasComponent<TagComponent>())
		{
			auto& component = entity.GetComponent<TagComponent>();

			std::string tag_str = TypeUtils::FromUTF16(component.Tag);

			char buffer[64];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag_str.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				component.Tag = TypeUtils::FromUTF8(buffer);
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (!SelectedEntity.HasComponent<CameraComponent>())
			{
				if (ImGui::MenuItem("Camera"))
				{
					SelectedEntity.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!SelectedEntity.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::MenuItem("Sprite Renderer"))
				{
					SelectedEntity.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!SelectedEntity.HasComponent<Rigidbody2DComponent>())
			{
				if (ImGui::MenuItem("Rigidbody 2D"))
				{
					SelectedEntity.AddComponent<Rigidbody2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!SelectedEntity.HasComponent<BoxCollider2DComponent>())
			{
				if (ImGui::MenuItem("Box Collider 2D"))
				{
					SelectedEntity.AddComponent<BoxCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform Component", entity, [](TransformComponent& component)
		{
			DrawVec3Control("Position", component.Position);
			DrawVec3Control("Rotation", component.Rotation);
			DrawVec3Control("Scale", component.Scale);
		});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer Component", entity, [](SpriteRendererComponent& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

			ImGui::Button("Texture", { 100.f, 0.f });

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;

					component.Texture = Texture2D::Create(path);
				}

				ImGui::EndDragDropTarget();
			}
		});

		DrawComponent<CameraComponent>("Camera Component", entity, [](CameraComponent& component)
		{
			auto& camera = component.Camera;

			static const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)(camera.GetProjectionType())];

			if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = !(bool)strcmp(currentProjectionTypeString, projectionTypeStrings[i]);
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.SetProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			switch (camera.GetProjectionType())
			{
				case SceneCamera::ProjectionType::Perspective:
				{
					float verticalFOV = camera.GetPerspectiveVerticalFOV();
					if (ImGui::DragFloat("Vertical FOV", &verticalFOV))
						camera.SetPerspectiveVerticalFOV(verticalFOV);

					float nearClip = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near Clip", &nearClip))
						camera.SetPerspectiveNearClip(nearClip);

					float farClip = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far Clip", &farClip))
						camera.SetPerspectiveFarClip(farClip);
				} break;
				case SceneCamera::ProjectionType::Orthographic:
				{
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						camera.SetOrthographicSize(orthoSize);

					float nearClip = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near Clip", &nearClip))
						camera.SetOrthographicNearClip(nearClip);

					float farClip = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far Clip", &farClip))
						camera.SetOrthographicFarClip(farClip);

				} break;
			}
		});

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
		{
			const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
			if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
					{
						currentBodyTypeString = bodyTypeStrings[i];
						component.Type = (Rigidbody2DComponent::BodyType)i;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
		});

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat2("Size", glm::value_ptr(component.Offset));
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
		});
	}
}
