#include "EditorLayer.h"

namespace Engine
{
	EditorLayer::EditorLayer() : m_CameraController(1280.f / 720.f), Layer("Sandbox2D")
	{
		
	}

	void EditorLayer::OnAttach()
	{
		Engine::FramebufferProps Props;
		Props.Width = 1280;
		Props.Height = 720;
		m_FrameBuffer = Engine::Framebuffer::Create(Props);

		m_SquareTexture = Engine::Texture2D::Create("assets/textures/cat.jpg");

		ActiveScene = CreateRef<Scene>();
		
		Square = ActiveScene->CreateEntity("Square Entity");
		Square.AddComponent<SpriteRendererComponent>(glm::vec4(0.f, 1.f, 0.f, 1.f));

		EditorCamera = ActiveScene->CreateEntity("Camera");
		EditorCamera.AddComponent<CameraComponent>();

		class CameraController : public ScriptableEntity
		{
		public:
			void OnCreate()
			{
				
			}

			void OnDestroy()
			{

			}

			void OnUpdate(Timestep DeltaTime)
			{
				auto& transform = GetComponent<TransformComponent>().Transform;
				float speed = 5.f;

				if (Input::IsKeyPressed(KeyInput::Key_A))
					transform[3][0] -= speed * DeltaTime;
				if (Input::IsKeyPressed(KeyInput::Key_D))
					transform[3][0] += speed * DeltaTime;
				if (Input::IsKeyPressed(KeyInput::Key_W))
					transform[3][1] += speed * DeltaTime;
				if (Input::IsKeyPressed(KeyInput::Key_S))
					transform[3][1] -= speed * DeltaTime;
			}
		};

		EditorCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(Engine::Timestep DeltaTime)
	{
		DE_PROFILE_FUNCTION();
		
		if(bUIShouldBlockEvents)
			m_CameraController.OnUpdate(DeltaTime);

		Engine::Renderer2D::ResetStats();

		m_FrameBuffer->Bind();

		ActiveScene->OnUpdate(DeltaTime);

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
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Editor"))
			{
				if (ImGui::MenuItem("Exit"))
					Engine::Application::Get().Close();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		auto stats = Engine::Renderer2D::GetStats();

		{
			ImGui::Begin("Settings");

			ImGui::Text("Renderer2D Stats:");
			ImGui::Text("Draw Calls: %i", stats.DrawCalls);
			ImGui::Text("Quad Count: %i", stats.QuadCount);
			ImGui::Text("Vertex Count: %i", stats.GetTotalVertexCount());
			ImGui::Text("Index Count: %i", stats.GetTotalIndexCount());
			
			if (Square.IsValid())
			{
				ImGui::Separator();
				ImGui::Text("%s", Square.GetComponent<TagComponent>().Tag.c_str());

				auto& SquareColor = Square.GetComponent<SpriteRendererComponent>().Color;

				ImGui::ColorEdit4("Square Color", glm::value_ptr(SquareColor));
				ImGui::Separator();
			}

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

				m_CameraController.OnResize(ViewportSize.x, ViewportSize.y);
				ActiveScene->OnViewportResize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			}

			bUIShouldBlockEvents = ImGui::IsWindowFocused();

			uint32_t BufferID = m_FrameBuffer->GetColorAttachmentRendererID();
			ImGui::Image((void*)BufferID, viewportPanelSize, ImVec2{ 0.f, 1.f }, ImVec2{ 1.f, 0.f });

			ImGui::End();
			ImGui::PopStyleVar();
		}

		ImGui::End();
	}

	void EditorLayer::OnEvent(Engine::Event& event)
	{
		m_CameraController.OnEvent(event);
	}
}