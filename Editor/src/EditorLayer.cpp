#include "EditorLayer.h"

#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Utils/PlatformUtils.h"

#include "ImGuizmo/ImGuizmo.h"

namespace Engine
{
	EditorLayer::EditorLayer() : Layer("Editor Layer")
	{
		
	}

	void EditorLayer::OnAttach()
	{
		Engine::FramebufferSpecification Specs;

		Specs.Attachments = {
			FramebufferTextureFormat::RGBA8,
			FramebufferTextureFormat::RED_INTEGER,
			FramebufferTextureFormat::Depth
		};

		Specs.Width = 1280;
		Specs.Height = 720;
		m_FrameBuffer = Engine::Framebuffer::Create(Specs);

		ActiveScene = CreateRef<Scene>();

		HPanel.SetContext(ActiveScene);

		editorCamera = EditorCamera(30.f, 16 / 9, 0.1f, 1000.f);
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(Engine::Timestep DeltaTime)
	{
		DE_PROFILE_FUNCTION();

		Engine::Renderer2D::ResetStats();

		m_FrameBuffer->Bind();

		Engine::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		Engine::RenderCommand::Clear();

		m_FrameBuffer->ClearAttachment(1, -1);

		editorCamera.OnUpdate(DeltaTime);

		ActiveScene->OnUpdateEditor(DeltaTime, editorCamera);

		auto [mx, my] = ImGui::GetMousePos();

		mx -= ViewportBounds[0].x;
		my -= ViewportBounds[0].y;
		glm::vec2 ViewportSize = ViewportBounds[1] - ViewportBounds[0];
		my = ViewportSize.y - my;

		int MouseX = (int)mx;
		int MouseY = (int)my;

		if (MouseX >= 0 && MouseY >= 0 && MouseX < (int)ViewportSize.x && MouseY < (int)ViewportSize.y)
		{
			int pixelData = m_FrameBuffer->ReadPixel(1, MouseX, MouseY);

			if (pixelData == -1)
				HoveredEntity = { };
			else
				HoveredEntity = { (entt::entity)pixelData, ActiveScene.get() };
		}

		m_FrameBuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender(Engine::Timestep DeltaTime)
	{
		DE_PROFILE_FUNCTION();

		static float t = 0;
		t += DeltaTime;

		static unsigned int fps = 0;

		if (t > 0.5f)
		{
			fps = (unsigned int)(1 / DeltaTime);
			t = 0.f;
		}

		static bool bIsDockSpaceOpen = true;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("DockSpace Demo", &bIsDockSpaceOpen, window_flags);

		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 320.f;

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Editor"))
			{
				if (ImGui::MenuItem("Exit"))
					Engine::Application::Get().Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New Scene", "Ctrl+N"))
				{
					ActiveScene = CreateRef<Scene>();
					ActiveScene->OnViewportResize((uint32_t)ViewportSize.x, (uint32_t)ViewportSize.y);
					HPanel.SetContext(ActiveScene);
				}

				if (ImGui::MenuItem("Open Scene", "Ctrl+O"))
				{
					std::string filepath = FileDialogs::OpenFile("Dragon Engine Scene (*.descene)\0*.descene\0");

					if (!filepath.empty())
					{
						ActiveScene = CreateRef<Scene>();
						ActiveScene->OnViewportResize((uint32_t)ViewportSize.x, (uint32_t)ViewportSize.y);
						HPanel.SetContext(ActiveScene);

						SceneSerializer s(ActiveScene);
						s.Deserialize(filepath);
					}
				}

				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
				{
					std::string filepath = FileDialogs::SaveFile("Dragon Engine Scene (*.descene)\0*.descene\0");

					if (!filepath.empty())
					{
						SceneSerializer s(ActiveScene);
						s.Serialize(filepath);
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		auto stats = Engine::Renderer2D::GetStats();

		{
			ImGui::Begin("Settings");

			const char* tag = HoveredEntity.IsValid() ? HoveredEntity.GetComponent<TagComponent>().Tag.c_str() : "None";

			ImGui::Text("Hovered Entity: %s", tag);
			ImGui::Text("Renderer2D Stats:");
			ImGui::Text("Draw Calls: %i", stats.DrawCalls);
			ImGui::Text("Quad Count: %i", stats.QuadCount);
			ImGui::Text("Vertex Count: %i", stats.GetTotalVertexCount());
			ImGui::Text("Index Count: %i", stats.GetTotalIndexCount());

			ImGui::Text("FPS: %i", fps);

			ImGui::End();
		}

		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.f, 0.f });
			ImGui::Begin("Viewport");

			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
			
			if (ViewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
			{
				m_FrameBuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
				ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

				ActiveScene->OnViewportResize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
				editorCamera.SetViewportSize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			}

			uint32_t BufferID = m_FrameBuffer->GetColorAttachmentRendererID();
			ImGui::Image((void*)BufferID, viewportPanelSize, ImVec2{ 0.f, 1.f }, ImVec2{ 1.f, 0.f });

			ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
			ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();

			ImVec2 viewportOffset = ImGui::GetWindowPos();

			ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

			//Gizmos
			Entity selectedEntity = HPanel.GetSelectedEntity();

			if (selectedEntity.IsValid())
			{
				ImGuizmo::SetOrthographic(false);

				ImGuizmo::SetDrawlist();

				ImGuizmo::SetRect(
					ViewportBounds[0].x,
					ViewportBounds[0].y,
					ViewportBounds[1].x - ViewportBounds[0].x,
					ViewportBounds[1].y - ViewportBounds[0].y
				);

				const glm::mat4& cameraProjection = editorCamera.GetProjection();
				glm::mat4 cameraView = editorCamera.GetViewMatrix();

				auto& tc = selectedEntity.GetComponent<TransformComponent>();
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

			ImGui::End();
			ImGui::PopStyleVar();
		}

		HPanel.OnImGuiRender();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Engine::Event& event)
	{
		EventDispatcher d(event);
		d.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnKeyPressedEvent));
		d.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonPressedEvent));

		editorCamera.OnEvent(event);
	}

	bool EditorLayer::OnKeyPressedEvent(KeyPressedEvent& event)
	{
		if (bIsViewportFocused)
		{
			if (event.GetRepeatCount() > 0)
				return false;

			switch (event.GetKey())
			{
			case KeyInput::Key_Q:
				GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				return true;
			case KeyInput::Key_W:
				GizmoType = ImGuizmo::OPERATION::ROTATE;
				return true;
			case KeyInput::Key_E:
				GizmoType = ImGuizmo::OPERATION::SCALE;
				return true;
			default:
				return false;
			}
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& event)
	{
		if (event.GetMouseButton() == MouseButtonInput::MouseButton_Left)
		{
			if(bIsViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(KeyInput::Key_LeftAlt))
				HPanel.SetSelectedEntity(HoveredEntity);

			return true;
		}

		return false;
	}
}