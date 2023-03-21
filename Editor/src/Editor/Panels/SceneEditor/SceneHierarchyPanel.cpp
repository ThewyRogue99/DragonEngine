#include "depch.h"
#include "SceneHierarchyPanel.h"

#include "PanelDragPayload.h"
#include "Editor/Asset/Serializer/Serializer.h"
#include "Engine/Asset/Serializer/Serializer.h"

#include "Engine/Scene/Components.h"

#include "Engine/Audio/AudioEngine.h"
#include "Engine/Scripting/ScriptEngine.h"

#include "Editor/Scene/EditorSceneManager.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Asset/TextureManager.h"

#include "Editor/Tools/ResourceTool.h"
#include "Editor/Tools/EditorTool.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Engine
{
	static Ref<Texture2D> CheckerboardTexture = nullptr;

	SceneHierarchyPanel::SceneHierarchyPanel() : EditorPanel("Scene Hierarchy")
	{

	}

	void SceneHierarchyPanel::OnCreate()
	{
		SetPanelStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));

		if(!CheckerboardTexture)
			CheckerboardTexture = ResourceTool::GetIcon(TEXT("Checkerboard"));

		Context = EditorSceneManager::GetEditorScene();

		EditorSceneManager::OnEditorSceneChange().AddCallback(BIND_CLASS_FN(SceneHierarchyPanel::OnEditorSceneChange));
		EditorTool::OnBeginPlay().AddCallback(BIND_CLASS_FN(SceneHierarchyPanel::OnBeginPlay));
		EditorTool::OnEndPlay().AddCallback(BIND_CLASS_FN(SceneHierarchyPanel::OnEndPlay));
	}

	void SceneHierarchyPanel::OnRender(float DeltaTime)
	{
		EditorPanel::OnRender(DeltaTime);

		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				Context->CreateEntity(TEXT("Empty Entity"));
			}

			ImGui::EndPopup();
		}

		auto& view = Context->SceneRegistry.view<TagComponent>();

		ImGuiListClipper clipper;
		clipper.Begin(view.size());

		while (clipper.Step())
		{
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			{
				// Get the entity ID
				Entity entity(*(std::next(view.begin(), i)), Context);

				auto& tag = entity.GetComponent<TagComponent>().Tag;

				std::string tag_str = TypeUtils::FromUTF16(tag);

				if (ImGui::Selectable(tag_str.c_str(), SelectedEntity == entity, ImGuiSelectableFlags_SpanAvailWidth))
				{
					SetSelectedEntity(entity);
				}

				if (ImGui::BeginPopupContextItem(entity.GetUUID().GetString().c_str()))
				{
					if (ImGui::MenuItem("Delete Entity"))
					{
						Context->DestroyEntity(entity);

						if (SelectedEntity == entity)
							SetSelectedEntity({ });
					}
					ImGui::EndPopup();
				}
			}
		}

		clipper.End();

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			SetSelectedEntity({ });

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
		if(Name == TEXT("SelectedEntity"))
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

		std::string tag_str = TypeUtils::FromUTF16(tag);

		if(ImGui::Selectable(tag_str.c_str(), SelectedEntity == entity, ImGuiSelectableFlags_SpanAvailWidth))
		{
			SetSelectedEntity(entity);
		}

		if (ImGui::BeginPopupContextItem(entity.GetUUID().GetString().c_str()))
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				Context->DestroyEntity(entity);

				if (SelectedEntity == entity)
					SetSelectedEntity({ });
			}
			ImGui::EndPopup();
		}
	}

	template<typename T>
	static void OnRemoveComponent(Entity entity)
	{
		entity.RemoveComponent<T>();
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
				ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 18);

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.13f, 0.13f, 0.13f, 1.f));
				if (ImGui::Button("...", ImVec2(20.f, 0.f)))
					ImGui::OpenPopup("ComponentSettings");
				ImGui::PopStyleColor();

				if (ImGui::BeginPopup("ComponentSettings"))
				{
					if (ImGui::MenuItem("Remove Component"))
						removeComponent = true;

					ImGui::EndPopup();
				}
			}

			if (open)
			{
				uiFunction(component);

				ImGui::TreePop();
			}

			if (removeComponent)
			{
				OnRemoveComponent<T>(entity);
			}
		}
	}

	static void DrawVec3Control(const char* label, glm::vec3& values, float resetValue = 0.f, float columnWidth = 70.f)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label);

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);

		ImGui::Text(label);
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);

		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);

		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);

		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;

		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}

	static void DrawFields(ScriptComponent& component)
	{
		Ref<Script>& ScriptRef = component.ScriptObject;
		if (ScriptRef)
		{
			for (auto& field : ScriptRef->GetFields())
			{
				ImGui::BeginDisabled(true);

				const ScriptFieldInfo* Info = field.GetInfo();
				const std::string& FieldName = Info->GetName();

				switch (Info->GetFieldType())
				{
				case Engine::ScriptFieldType::Float:
				{
					float buff;
					field.Get(&buff);

					ImGui::DragFloat(FieldName.c_str(), &buff);
				} break;
				case Engine::ScriptFieldType::Double:
					break;
				case Engine::ScriptFieldType::Bool:
				{
					bool buff;
					field.Get(&buff);

					ImGui::Checkbox(FieldName.c_str(), &buff);
				} break;
				case Engine::ScriptFieldType::Char:
					break;
				case Engine::ScriptFieldType::Short:
					break;
				case Engine::ScriptFieldType::Int:
				{
					int buff;
					field.Get(&buff);

					ImGui::DragInt(FieldName.c_str(), &buff);
				} break;
				case Engine::ScriptFieldType::Long:
					break;
				case Engine::ScriptFieldType::Byte:
					break;
				case Engine::ScriptFieldType::UShort:
					break;
				case Engine::ScriptFieldType::UInt:
					break;
				case Engine::ScriptFieldType::ULong:
					break;
				case Engine::ScriptFieldType::Vector2:
				{
					glm::vec2 buff;
					field.Get(&buff);

					ImGui::DragFloat2(FieldName.c_str(), glm::value_ptr(buff));
				} break;
				case Engine::ScriptFieldType::Vector3:
				{
					glm::vec3 buff;
					field.Get(&buff);

					ImGui::DragFloat3(FieldName.c_str(), glm::value_ptr(buff));
				} break;
				case Engine::ScriptFieldType::Vector4:
				{
					glm::vec4 buff;
					field.Get(&buff);

					ImGui::DragFloat4(FieldName.c_str(), glm::value_ptr(buff));
				} break;
				case Engine::ScriptFieldType::Entity:
					break;
				default:
					break;
				}

				ImGui::EndDisabled();
			}
		}
		else
		{
			MemoryMap& Fields = component.Fields;
			if (Fields.size() > 0)
			{
				std::vector<ScriptFieldInfo> FieldInfos = ScriptEngine::GetScriptInfo(
					component.Namespace,
					component.Name
				)->GetScriptFieldInfos();

				for (auto& Info : FieldInfos)
				{
					const std::string& FieldName = Info.GetName();
					MemoryMap::FieldData FData = Fields.GetField<MemoryMap>(FieldName).GetFieldData("Data");

					switch (Info.GetFieldType())
					{
					case Engine::ScriptFieldType::Float:
					{
						float* buff = (float*)FData.DataPtr;

						ImGui::DragFloat(FieldName.c_str(), buff);
					} break;
					case Engine::ScriptFieldType::Double:
						break;
					case Engine::ScriptFieldType::Bool:
					{
						bool* buff = (bool*)FData.DataPtr;

						ImGui::Checkbox(FieldName.c_str(), buff);
					} break;
					case Engine::ScriptFieldType::Char:
						break;
					case Engine::ScriptFieldType::Short:
						break;
					case Engine::ScriptFieldType::Int:
					{
						int* buff = (int*)FData.DataPtr;

						ImGui::DragInt(FieldName.c_str(), buff);
					} break;
					case Engine::ScriptFieldType::Long:
						break;
					case Engine::ScriptFieldType::Byte:
						break;
					case Engine::ScriptFieldType::UShort:
						break;
					case Engine::ScriptFieldType::UInt:
						break;
					case Engine::ScriptFieldType::ULong:
						break;
					case Engine::ScriptFieldType::Vector2:
					{
						glm::vec2* buff = (glm::vec2*)FData.DataPtr;

						ImGui::DragFloat2(FieldName.c_str(), glm::value_ptr(*buff));
					} break;
					case Engine::ScriptFieldType::Vector3:
					{
						glm::vec3* buff = (glm::vec3*)FData.DataPtr;

						ImGui::DragFloat3(FieldName.c_str(), glm::value_ptr(*buff));
					} break;
					case Engine::ScriptFieldType::Vector4:
					{
						glm::vec4* buff = (glm::vec4*)FData.DataPtr;

						ImGui::DragFloat4(FieldName.c_str(), glm::value_ptr(*buff));
					} break;
					case Engine::ScriptFieldType::Entity:
						break;
					default:
						break;
					}
				}
			}
		}
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
			{
				component.Tag = TypeUtils::FromUTF8(buffer);
			}
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

			if (!SelectedEntity.HasComponent<CircleRendererComponent>())
			{
				if (ImGui::MenuItem("Circle Renderer"))
				{
					SelectedEntity.AddComponent<CircleRendererComponent>();
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

			if (!SelectedEntity.HasComponent<CircleCollider2DComponent>())
			{
				if (ImGui::MenuItem("Circle Collider 2D"))
				{
					SelectedEntity.AddComponent<CircleCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!SelectedEntity.HasComponent<AudioComponent>())
			{
				if (ImGui::MenuItem("Audio Component"))
				{
					SelectedEntity.AddComponent<AudioComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			if (!SelectedEntity.HasComponent<ScriptComponent>())
			{
				if (ImGui::MenuItem("Script"))
				{
					SelectedEntity.AddComponent<ScriptComponent>();
					ImGui::CloseCurrentPopup();
				}
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](TransformComponent& component)
		{
			DrawVec3Control("Position", component.Position);
			DrawVec3Control("Rotation", component.Rotation);
			DrawVec3Control("Scale", component.Scale, 1.f);
		});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

			ImGui::Text("Texture: ");
			ImGui::ImageButton(
				component.Texture
				? (ImTextureID)(component.Texture->GetRendererID())
				: (ImTextureID)(CheckerboardTexture->GetRendererID()),
				ImVec2(128, 128),
				ImVec2(0.f, 1.f),
				ImVec2(1.f, 0.f)
			);

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");

				if (payload && payload->IsDelivery())
				{
					PanelDragPayload::ContentBrowserItem Item;
					Item.FromData((uint8_t*)payload->Data);

					if (Item.ItemType == AssetType::Texture)
					{
						component.TextureID = Item.GetID();
						component.Texture = TextureManager::LoadTexture(component.TextureID);
					}
				}

				ImGui::EndDragDropTarget();
			}

			if (component.Texture)
			{
				ImGui::SameLine(0.f, 20.f);
				if (ImGui::Button("Remove Texture"))
				{
					component.TextureID = "";
					component.Texture = nullptr;
				}
			}
		});

		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](CircleRendererComponent& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.f, 1.f);
			ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.f, 1.f);
		});

		DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& component)
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

			ImGui::Checkbox("Primary", &component.Primary);
		});

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](Rigidbody2DComponent& component)
		{
			const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
			if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
			{
				for (int i = 0; i < 3; i++)
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

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](BoxCollider2DComponent& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
		});

		DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](CircleCollider2DComponent& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat("Radius", &component.Radius);
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
		});

		DrawComponent<ScriptComponent>("Script", entity, [](ScriptComponent& component)
		{
			auto& ScriptInfoList = ScriptEngine::GetScriptInfoList();

			size_t size = ScriptInfoList.size();
			std::vector<std::string> FullScriptNameList(size);

			for (size_t i = 0; i < size; i++)
			{
				auto& info = ScriptInfoList[i];

				auto& Namespace = info->GetNameSpace();
				auto& Name = info->GetName();

				FullScriptNameList[i] = (Namespace.empty() ? "" : Namespace + ".") + Name;
			}

			std::string componentScriptFullName = (component.Namespace.empty() ? "" : component.Namespace + ".") + component.Name;
			const char* preview = component.Name.empty() ? "Select Script..." : componentScriptFullName.c_str();

			if (ImGui::BeginCombo("Scripts", preview))
			{
				for (size_t i = 0; i < size; i++)
				{
					bool selected = FullScriptNameList[i] == preview;

					if (ImGui::Selectable(FullScriptNameList[i].c_str(), selected))
					{
						component.Name = ScriptInfoList[i]->GetName();
						component.Namespace = ScriptInfoList[i]->GetNameSpace();

						ScriptEngine::GetScriptDefaultFields(component.Namespace, component.Name, component.Fields);
					}
				}

				ImGui::EndCombo();
			}

			DrawFields(component);
		});

		DrawComponent<AudioComponent>("Audio Component", entity, [](AudioComponent& component)
		{
			ImGui::Button("Audio", { 100.f, 0.f });
			
			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");

				if (payload && payload->IsDelivery())
				{
					PanelDragPayload::ContentBrowserItem Item;
					Item.FromData((uint8_t*)payload->Data);

					if (Item.ItemType == AssetType::Audio)
					{
						Ref<Asset> AudioAsset = AssetManager::LoadAsset(Item.GetID());

						component.AudioID = Item.GetID();
						if(!component.Source)
							component.Source = AudioEngine::CreateAudioSource();

						Ref<AudioBuffer> Buff = AudioBuffer::Create();
						Buff->SetAudioEffect(Serializer::DeserializeAudio(AudioAsset->Metadata));

						component.Source->SetBuffer(Buff);
					}
				}

				ImGui::EndDragDropTarget();
			}

			if (component.Source)
			{
				ImGui::SameLine(0.f, 20.f);
				if (ImGui::Button("Play", { 50.f, 0.f }))
					AudioEngine::PlayAudio(component.Source);

				ImGui::SameLine(0.f, 5.f);
				if (ImGui::Button("Stop", { 50.f, 0.f }))
					AudioEngine::StopAudio(component.Source);

				ImGui::DragFloat3("Offset", glm::value_ptr(component.Offset));

				float Gain = component.Source->GetGain();
				if (ImGui::DragFloat("Gain", &Gain, 0.1f, 0.f, 1.f))
				{
					component.Source->SetGain(Gain);
				}

				float Pitch = component.Source->GetPitch();
				if (ImGui::DragFloat("Pitch", &Pitch, 0.1f, 0.f, 1.f))
				{
					component.Source->SetPitch(Pitch);
				}

				bool Loop = component.Source->GetLoopSound();
				if (ImGui::Checkbox("Loop Sound", &Loop))
				{
					component.Source->SetLoopSound(Loop);
				}
			}
		});
	}

	void SceneHierarchyPanel::OnEditorSceneChange(EditorScene* NewScene)
	{
		if (!EditorTool::IsPlaying())
			Context = NewScene;
	}

	void SceneHierarchyPanel::OnBeginPlay()
	{
		SceneManager::OnSetActiveScene().AddCallback([&] (Scene* NewScene) { Context = NewScene; });

		Context = SceneManager::GetActiveScene();
		SelectedEntity = { };
	}

	void SceneHierarchyPanel::OnEndPlay()
	{
		Context = EditorSceneManager::GetEditorScene();
		SelectedEntity = { };
	}
}
