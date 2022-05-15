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

		m_VertexArray = Engine::VertexArray::Create();

		float SquareVertices[] = {
			-0.5f, -0.5f,  0.0f,
			 0.5f, -0.5f,  0.0f,
			 0.5f,  0.5f,  0.0f,
			-0.5f,  0.5f,  0.0f
		};

		auto VertexBuffer = Engine::VertexBuffer::Create(SquareVertices, sizeof(SquareVertices));

		VertexBuffer->SetLayout({
			{ Engine::ShaderDataType::Float3, "a_Position" }
			});

		m_VertexArray->AddVertexBuffer(VertexBuffer);

		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
		auto IndexBuffer = Engine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

		m_VertexArray->SetIndexBuffer(IndexBuffer);

		m_Shader = Engine::Shader::Create(
			"Shader",
			"../Engine/src/Engine/Renderer/Shaders/MaterialVertex.glsl",
			"../Engine/src/Engine/Renderer/Shaders/MaterialFragment.glsl",
			true
		);

		m_SquareTexture = Engine::Texture2D::Create("assets/textures/cat.jpg");
	}

	void EditorLayer::OnDetach()
	{

	}

	void EditorLayer::OnUpdate(Engine::Timestep DeltaTime)
	{
		DE_PROFILE_FUNCTION();

		{
			DE_PROFILE_SCOPE("CameraController::OnUpdate");

			m_CameraController.OnUpdate(DeltaTime);
		}

		Engine::Renderer2D::ResetStats();
		{
			DE_PROFILE_SCOPE("Renderer Prep");

			m_FrameBuffer->Bind();

			Engine::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
			Engine::RenderCommand::Clear();
		}

		{
			DE_PROFILE_SCOPE("Renderer Draw");

			Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());

			static float rotation = 0.f;

			rotation += DeltaTime * SquareRotationSpeed * 10.f;

			Engine::Renderer2D::RenderProperties props;
			props.TilingFactor = 10.f;

			Engine::Renderer2D::DrawQuad(glm::vec3(0.f, 0.f, -0.1f), glm::vec2(20.f), m_SquareTexture, props);

			Engine::Renderer2D::DrawRotatedQuad({ SquarePosition.x, SquarePosition.y, 0.f }, rotation, SquareSize, SquareColor);
			Engine::Renderer2D::DrawQuad(glm::vec3(2.f, 0.f, 0.f), glm::vec2(1.f), glm::vec4(1.f, 0.f, 0.f, 1.f));
			Engine::Renderer2D::DrawRotatedQuad(glm::vec3(2.f, -1.f, 0.f), 45.f, glm::vec2(1.f), glm::vec4(0.f, 0.f, 1.f, 1.f));
			Engine::Renderer2D::DrawQuad(glm::vec3(-2.f, 0.f, 0.f), glm::vec2(1.f), glm::vec4(1.f, 0.f, 0.f, 1.f));

			Engine::Renderer2D::EndScene();

			Engine::Renderer2D::BeginScene(m_CameraController.GetCamera());

			for (float y = -5.f; y < 5.f; y += 0.5f)
			{
				for (float x = -5.f; x < 5.f; x += 0.5f)
				{
					glm::vec4 color = { (x + 5.f) / 10.f, 0.4f, (y + 5.f) / 10.f, 0.7f };
					Engine::Renderer2D::DrawQuad({ x, y, 0.f }, { 0.45f, 0.45f }, color);
				}
			}

			Engine::Renderer2D::EndScene();
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

		static bool dosckspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("DockSpace Demo", &dosckspaceOpen, window_flags);

		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
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

			ImGui::ColorEdit4("Square Color", glm::value_ptr(SquareColor));
			ImGui::DragFloat2("Square Position", glm::value_ptr(SquarePosition), 0.1f);
			ImGui::DragFloat2("Square Size", glm::value_ptr(SquareSize), 0.1f);
			ImGui::DragFloat("Square Rotation Speed", &SquareRotationSpeed, 0.05f, 0.f, 10.f);

			ImGui::Text("FPS: %i", fps);

			uint32_t BufferID = m_FrameBuffer->GetColorAttachmentRendererID();

			ImGui::Image((void*)BufferID, ImVec2{ 1280.f, 720.f }, ImVec2{ 0.f, 1.f }, ImVec2{ 1.f, 0.f });

			ImGui::End();
		}

		ImGui::End();
	}

	void EditorLayer::OnEvent(Engine::Event& event)
	{
		m_CameraController.OnEvent(event);
	}
}